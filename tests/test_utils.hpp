#pragma once

#include <codeowners/filesystem.hpp>

#include <gtest/gtest.h>

#include <boost/process.hpp>

namespace co
{

struct git_invoker
{
    git_invoker(fs::path repository_root)
        : m_repository_root{std::move(repository_root)} {};

    template <typename... Args>
    void operator()(Args&&... args)
    {
        boost::process::system(boost::process::search_path("git"), std::forward<Args>(args)...,
                               boost::process::start_dir = m_repository_root.string(),
                               boost::process::std_out > boost::process::null,
                               boost::process::std_err > boost::process::null,
                               boost::process::throw_on_error);
    }

private:
    fs::path m_repository_root;
};

/// Return whether the two paths are equivalent, i.e. refer to the same
/// filesystem entity.
inline ::testing::AssertionResult equivalent(const fs::path& p1, const fs::path& p2)
{
    // NOTE: Google Test bug #1614 applies when printing two fs::path objects.
    // https://github.com/google/googletest/issues/1614
    if (fs::equivalent(p1, p2))
    {
        return ::testing::AssertionSuccess() << "Path " << p1 << " is equivalent to " << p2;
    }
    else
    {
        return ::testing::AssertionFailure() << "Path " << p1 << " is not equivalent to " << p2;
    }
}

#define EXPECT_PATHS_EQUIVALENT(p1, p2) EXPECT_TRUE(co::equivalent((p1), (p2)))

} // end namespace 'co'