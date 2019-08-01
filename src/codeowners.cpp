#include <codeowners/codeowners.hpp>
#include <codeowners/errors.hpp>
#include <codeowners/filesystem.hpp>

#include <git2/index.h>
#include <git2/pathspec.h>
#include <git2/repository.h>

#include <iostream>
#include <memory>

namespace co
{


std::optional<fs::path> codeowners_file(const fs::path& repo_root)
{
    const char* basename = "CODEOWNERS";
    for (const auto& dir : { "", "docs", ".github" })
    {
        fs::path prospective_path = repo_root / dir / basename;
        if (fs::exists(prospective_path))
        {
            return prospective_path;
        }
    }
    return std::nullopt;
}


using repository_ptr = std::unique_ptr<::git_repository, void (*)(::git_repository*)>;
using index_ptr = std::unique_ptr<::git_index, void (*)(::git_index*)>;

template <typename T>
struct resource_traits;

#define SPECIALIZE_RESOURCE_TRAITS(RESOURCE)                     \
    template <>                                                  \
    struct resource_traits<RESOURCE>                             \
    {                                                            \
        using value_type = RESOURCE;                             \
        using deleter_type = void (*)(value_type*);              \
        constexpr static deleter_type deleter = RESOURCE##_free; \
        constexpr static const char* resource_name = #RESOURCE;  \
    };

SPECIALIZE_RESOURCE_TRAITS(git_repository);
SPECIALIZE_RESOURCE_TRAITS(git_index);
SPECIALIZE_RESOURCE_TRAITS(git_pathspec);

#undef SPECIALIZE_RESOURCE_TRAITS

template <typename T>
using resource_ptr = std::unique_ptr<T, typename resource_traits<T>::deleter_type>;

template <typename T, typename F, typename... Args>
resource_ptr<T> make_resource_ptr(F f, Args... args)
{
    using traits = resource_traits<T>;
    T* resource = nullptr;
    int error = f(&resource, std::forward<Args>(args)...);
    if (error)
    {
        using namespace std::string_literals;
        throw co::error { "Error while creating "s + traits::resource_name
            + ": error code " + std::to_string(error) };
    }
    assert(resource);
    return resource_ptr<T>(resource, resource_traits<T>::deleter);
};

repository_ptr make_repository_ptr(const fs::path& repo_root)
{
    ::git_repository* repo = nullptr;
    int error = ::git_repository_open_ext(&repo, repo_root.c_str(), /*flags*/ 0, /*ceiling dirs*/ nullptr);
    if (error)
    {
        using namespace std::string_literals;
        throw repository_not_found_error { "Could not find git repository at "s + repo_root.string() };
    }
    assert(repo);
    return repository_ptr(repo, ::git_repository_free);
}

index_ptr make_index_ptr(::git_repository* repo)
{
    assert(repo);
    ::git_index* index = nullptr;
    int error = ::git_repository_index(&index, repo);
    if (error)
    {
        throw co::error { "Error getting index: libgit2 error code: " + std::to_string(error) };
    }
    assert(index);
    return index_ptr(index, ::git_index_free);
}

struct repository_impl
{
    repository_impl(const fs::path& repo_root)
        : m_root(repo_root)
        , m_repo_ptr(make_repository_ptr(repo_root))
        , m_index_ptr(make_index_ptr(m_repo_ptr.get()))
    {
    }

    bool contains(const fs::path& path) const
    {
        fs::path relative_path = fs::relative(path, m_root);
        auto* entry = ::git_index_get_bypath(m_index_ptr.get(), relative_path.c_str(), /* stage */ GIT_INDEX_STAGE_NORMAL);
        return entry != nullptr;
    }

    std::vector<fs::path> index_paths() const
    {
        std::size_t entry_count = ::git_index_entrycount(m_index_ptr.get());
        std::vector<fs::path> paths;
        paths.reserve(entry_count);
        for (std::size_t i = 0; i < entry_count; i++)
        {
            auto* entry = ::git_index_get_byindex(index(), i);
            paths.emplace_back(entry->path);
        }
        return paths;
    }

private:
    ::git_index* index() const { return m_index_ptr.get(); }

    fs::path m_root;
    repository_ptr m_repo_ptr;
    index_ptr m_index_ptr;
};

repository::repository(const fs::path& repository_root)
    : m_impl { std::make_unique<repository_impl>(repository_root) }
{
}

repository::~repository() = default;

const repository_impl* repository::impl() const { return m_impl.get(); }
repository_impl* repository::impl() { return m_impl.get(); }

bool repository::contains(const fs::path& path) const { return m_impl->contains(path); }
std::vector<fs::path> repository::index_paths() const { return impl()->index_paths(); }

bool file_pattern::match(const char* path) const
{
    char* pattern_data[] = { const_cast<char*>(pattern.data()) };  // Promise this is okay.

    ::git_strarray pattern_strarray { pattern_data, 1 };
    auto pathspec_ptr = make_resource_ptr<::git_pathspec>(::git_pathspec_new, &pattern_strarray);

    const bool matched = ::git_pathspec_matches_path(pathspec_ptr.get(), GIT_PATHSPEC_DEFAULT, path);
    return matched ^ invert;
}

} /* end namespace 'co' */
