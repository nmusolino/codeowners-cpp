#pragma once

#include "codeowners/paths.hpp"
#include <memory>

namespace co {

class repository_impl;

class error : public std::exception
{
public:
    error(const std::string& message) : m_message{message} {};
    const char* what() const noexcept override { return m_message.c_str(); }
private:
    std::string m_message;
};

class repository_not_found_error : public error
{
public:
    using error::error;
};


class repository {
public:
    repository(const fs::path& repository_root);
    ~repository();

    bool contains(const fs::path& path) const;
    std::vector<fs::path> index_paths() const;
private:
    const repository_impl* impl() const;
    repository_impl* impl();

    std::unique_ptr<repository_impl> m_impl;
};

} /* end namespace 'co' */
