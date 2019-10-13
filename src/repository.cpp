#include <codeowners/errors.hpp>
#include <codeowners/repository.hpp>

#include "git_resources.hpp"

#include <git2/errors.h> // libgit2 error codes
#include <git2/repository.h>
#include <git2/submodule.h>

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/concat.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/single.hpp>
#include <range/v3/view/transform.hpp>

namespace co
{

static constexpr std::array<const char*, 3> codeowner_relative_paths{
    "CODEOWNERS", "docs/CODEOWNERS", ".github/CODEOWNERS"};

/* static member functions */
repository repository::create(const fs::path& path)
{
    return create(path, repo_structure::NONBARE);
}

repository repository::create(const fs::path& path, repo_structure structure)
{
    return make_resource_ptr<::git_repository>(::git_repository_init, path.c_str(),
                                               /*bare*/ structure == repo_structure::BARE);
}

repository repository::open(const fs::path& path)
{
    return make_resource_ptr<::git_repository>(::git_repository_open, path.c_str());
}

repository repository::discover(const fs::path& start_point)
{
    return discover(start_point, discovery_strategy::NEVER_CROSS_FILESYSTEMS);
}

repository repository::discover(const fs::path& start_point, discovery_strategy strategy)
{
    auto maybe_repo = try_discover(start_point, strategy);
    if (!maybe_repo)
    {
        using namespace std::string_literals;
        throw repository_not_found_error{"Repository not found from start point: "s
                                         + start_point.c_str()};
    }
    return *std::move(maybe_repo);
}

std::optional<repository> repository::try_discover(const fs::path& start_point)
{
    return try_discover(start_point, discovery_strategy::NEVER_CROSS_FILESYSTEMS);
}

std::optional<repository> repository::try_discover(const fs::path& start_point,
                                                   discovery_strategy strategy)
{
    if (!fs::exists(start_point))
    {
        using namespace std::string_literals;
        throw file_not_found_error{"File not found: "s + start_point.c_str()};
    }
    git_buffer buf;
    bool cross_filesystems = (strategy == discovery_strategy::CROSS_FILESYSTEMS);
    int err = ::git_repository_discover(&buf, start_point.c_str(), cross_filesystems,
                                        /* ceiling dirs */ nullptr);

    switch (err)
    {
    case 0:
    {
        assert(buf.data());
        auto path = fs::canonical(buf.data());
        return repository::open(path);
    }
    case GIT_ENOTFOUND:
    {
        return std::nullopt;
    }
    default:
    {
        // Unclear if libgit2 ever reports other errors.
        using namespace std::string_literals;
        throw error{"Error while attempting to discover git repository from "s
                    + start_point.c_str()};
    }
    }
    assert(false && "Unreachable");
    return std::nullopt;
}

fs::path repository::common_directory() const
{
    const char* p = ::git_repository_commondir(raw());
    assert(p);
    return fs::path(p);
}

fs::path repository::work_directory() const
{
    const char* p = ::git_repository_workdir(raw());
    assert(p); // Might fail for bare repo?
    return p;
}

fs::path repository::git_directory() const
{
    const char* p = ::git_repository_path(raw());
    assert(p);
    return p;
}

bool repository::is_bare() const { return ::git_repository_is_bare(raw()); }

bool repository::is_empty() const
{
    return ::git_repository_is_empty(const_cast<::git_repository*>(raw()));
}

std::vector<fs::path> repository::submodule_paths() const
{
    std::vector<fs::path> paths;

    const auto callback = [](git_submodule* sm, const char* /*name*/, void* payload) -> int {
        auto& paths = *reinterpret_cast<std::vector<fs::path>*>(payload);
        paths.emplace_back(::git_submodule_path(sm));
        return 0;
    };
    ::git_submodule_foreach(const_cast<::git_repository*>(raw()), callback, &paths);
    return paths;
}

::git_repository* repository::raw() { return m_ptr.get(); }

const ::git_repository* repository::raw() const { return m_ptr.get(); }

std::optional<fs::path> codeowners_path(const fs::path& work_directory)
{
    for (const auto& rel_path : codeowner_relative_paths)
    {
        if (fs::path p = work_directory / rel_path; fs::exists(p))
        {
            return p;
        }
    }
    return std::nullopt;
}

std::vector<fs::path> nonwork_directories(const repository& repo)
{
    fs::path work_dir = repo.work_directory();
    auto subm_paths = repo.submodule_paths();
    auto full_subm_paths = subm_paths
        | ranges::views::transform([&work_dir](const auto& p) { return work_dir / p; })
        | ranges::views::filter([](const auto& p) { return fs::exists(p); });
    auto git_dir = ranges::views::single(repo.git_directory());
    return ranges::views::concat(git_dir, full_subm_paths) | ranges::to<std::vector<fs::path>>();
}

} // end namespace 'co'
