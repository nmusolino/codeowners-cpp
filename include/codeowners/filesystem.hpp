#pragma once

#include <boost/filesystem.hpp>
#include <boost/process.hpp>

#include <fstream>
#include <vector>

namespace fs = boost::filesystem;

namespace co
{

inline void ensure_exists(const fs::path& p) { std::ofstream ofs{p.string(), std::ios::app}; }

/**
 * Return a subset of `paths` such that the recursive iteration over each path in the
 * returned container produces disjoint sets of paths.
 *
 * This function processes `paths`, and removes any elements for which a strict prefix is also
 * contained in `paths`.  For example, if the input is ["x", "x/y", "x/y/z", "w/v"], the output
 * would be ["x", "w/v"], because "x/y"
 */
std::vector<fs::path> distinct_prefixed_paths(std::vector<fs::path> paths);

struct temporary_directory_handle : public boost::noncopyable
{
    using path_type = fs::path;

    temporary_directory_handle()
        : m_path{create_temporary_directory()}
    {
    }

    temporary_directory_handle(const temporary_directory_handle&) = delete;
    temporary_directory_handle(temporary_directory_handle&& other) noexcept
        : m_path{}
    {
        swap(other);
    }

    temporary_directory_handle& operator=(const temporary_directory_handle& other) = delete;
    temporary_directory_handle& operator=(temporary_directory_handle&& other) noexcept
    {
        swap(other);
        return *this;
    }

    ~temporary_directory_handle()
    {
        if (!m_path.empty())
        {
            // Ignore errors to prevent an exception propagating outside
            // destructor.
            boost::system::error_code ec;
            fs::remove_all(m_path, ec);
        }
    }

    operator path_type() const { return path(); }
    path_type path() const { return m_path; }
    std::string path_str() const { return m_path.string(); }
    const char* c_str() const { return m_path.c_str(); }

    fs::path operator/(const fs::path& p) const { return m_path / p; }
    fs::path operator/(const std::string& p) const { return m_path / p; }
    fs::path operator/(const char* p) const { return m_path / p; }

    void swap(temporary_directory_handle& other) noexcept { m_path.swap(other.m_path); }

private:
    static fs::path create_temporary_directory()
    {
        fs::path path
            = fs::temp_directory_path() / fs::unique_path("codeowners-%%%%-%%%%-%%%%-%%%%");
        fs::create_directories(path);
        return path;
    }

private:
    path_type m_path;
};

inline void swap(temporary_directory_handle& a, temporary_directory_handle& b) noexcept
{
    a.swap(b);
}

} // end namespace 'co'
