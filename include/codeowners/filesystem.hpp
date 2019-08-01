#pragma once

#include <boost/filesystem.hpp>
#include <boost/process.hpp>

#include <fstream>

namespace fs = boost::filesystem;

namespace co
{

inline void ensure_exists(const fs::path& p)
{
    std::ofstream ofs { p.string() };
}

struct temporary_directory_handle
{
    using path_type = fs::path;

    temporary_directory_handle()
        : m_path { create_temporary_directory() }
    {
    }

    ~temporary_directory_handle()
    {
        // Ignore errors to prevent an exception propagating outside destructor.
        boost::system::error_code ec;
        fs::remove_all(m_path, ec);
    }

    path_type path() const { return m_path; }
    std::string path_str() const { return m_path.string(); }

private:
    static fs::path create_temporary_directory()
    {
        fs::path path = fs::temp_directory_path() / fs::unique_path("codeowners-%%%%-%%%%-%%%%-%%%%");
        fs::create_directories(path);
        return path;
    }

private:
    path_type m_path;
};

} /* end namespace 'co' */
