#pragma once

#include "codeowners/filesystem.hpp"
#include "codeowners/git_resources_fwd.hpp"
#include "codeowners/type_utils.hpp"

#include <memory>
#include <optional>
#include <vector>

namespace co
{

enum class discovery_strategy
{
    NEVER_CROSS_FILESYSTEMS,
    CROSS_FILESYSTEMS
};

enum class repo_structure
{
    NONBARE,
    BARE
};

class repository
{
public:
    /// Create a new, non-bare repository at the given path.
    static repository create(const fs::path& path);
    static repository create(const fs::path& path, repo_structure structure);

    /// Open a repository, which must exist at the given path.
    static repository open(const fs::path& path);

    /// Discover the repository
    static repository discover(const fs::path& start_point);
    static repository discover(const fs::path& start_point, discovery_strategy strategy);
    static std::optional<repository> try_discover(const fs::path& start_point);
    static std::optional<repository> try_discover(const fs::path& start_point,
                                                  discovery_strategy strategy);

    fs::path common_directory() const;
    fs::path work_directory() const;
    fs::path git_directory() const;
    /// Return whether the repository is a bare repository.
    bool is_bare() const;
    /// Return whether the repository is empty.
    bool is_empty() const;

    std::vector<fs::path> submodule_paths() const;

private:
    repository(owning_ptr<::git_repository>&& ptr)
        : m_ptr{std::move(ptr)}
    {
    }

    /// Return a pointer to the libgit2 repository object.
    ::git_repository* raw();
    const ::git_repository* raw() const;

private:
    owning_ptr<::git_repository> m_ptr;
};

std::optional<fs::path> codeowners_path(const fs::path& work_directory);

/**
 * Return the git directory and any submodule paths.  This set of directories
 * should generally be skipped when iterating over files.
 */
std::vector<fs::path> nonwork_directories(const repository&);

} // end namespace 'co'