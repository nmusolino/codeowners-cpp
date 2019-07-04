#pragma once

#include <boost/filesystem.hpp>
#include <boost/process.hpp>

#include <fstream>

namespace fs = boost::filesystem;

namespace co {

fs::path touch(const fs::path& p)
{
    std::ofstream ofs { p.string() };
    return p;
}

} /* end namespace 'co' */
