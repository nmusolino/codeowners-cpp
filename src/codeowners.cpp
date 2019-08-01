#include <codeowners/codeowners.hpp>
#include <codeowners/errors.hpp>
#include <codeowners/filesystem.hpp>


#include <iostream>
#include <memory>

namespace co
{

/*
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
        auto* entry = ::git_index_get_bypath(m_index_ptr.get(), relative_path.c_str(), GIT_INDEX_STAGE_NORMAL);
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
*/

}  // end namespace 'co'
