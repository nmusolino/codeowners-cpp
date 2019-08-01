#pragma once

#include "codeowners/filesystem.hpp"

#include <memory>
#include <optional>

namespace co
{

struct repository_impl;


class repository
{
public:
    repository(const fs::path& repository_root);
    ~repository();
    static repository find(const fs::path& path);

    bool contains(const fs::path& path) const;
    std::vector<fs::path> index_paths() const;

private:
    const repository_impl* impl() const;
    repository_impl* impl();

    std::unique_ptr<repository_impl> m_impl;
};

std::optional<fs::path> codeowners_file(const fs::path& repo_root);

struct file_pattern
{
    const std::string pattern;
    const bool invert = false;

    bool match(const fs::path& path) const { return match(path.c_str()); }
    bool match(const std::string& path) const { return match(path.c_str()); }
    bool match(const char* path) const;
};

struct owner_rule
{
    file_pattern pattern;
    std::vector<std::string> owners;
};

}  // end namespace 'co'
