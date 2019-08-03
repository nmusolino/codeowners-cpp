#pragma once

#include "codeowners/filesystem.hpp"
#include "codeowners/git_resources.hpp"

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
    /// Discover the repository
    static repository discover(const fs::path& start_point);
    static repository discover(const fs::path& start_point,
                               discovery_strategy strategy);
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

    /// Return a pointer to the libgit2 repository object.
    git_repository* raw();
    const git_repository* raw() const;

private:
    repository_ptr m_ptr;
};

} // end namespace 'co'