#include <codeowners/filesystem.hpp>
#include <range/v3/action/sort.hpp>
#include <range/v3/view/unique.hpp>

#include <algorithm>
#include <iostream>

namespace co
{
namespace
{

    bool share_prefix(const fs::path& p, const fs::path& q)
    {
        auto [p_it, q_it] = std::mismatch(p.begin(), p.end(), q.begin(), q.end());
        return p_it == p.end();
    }

} // end anonymous namespace

std::vector<fs::path> distinct_prefixed_paths(std::vector<fs::path> paths)
{
    std::sort(paths.begin(), paths.end());

    // De-duplicate paths, in the spirit of `std::unique`.  Using `std::unique` itself is not
    // allowed, because `share_prefix` is not an equivalence relation (it is not symmetric).
    auto it = paths.begin();
    auto logical_end = paths.end();
    while (it != logical_end)
    {
        auto region_end = std::find_if_not(it, logical_end,
                                           [&](const fs::path& q) { return share_prefix(*it, q); });
        logical_end = std::rotate(++it, region_end, logical_end);
    }
    paths.erase(logical_end, paths.end());

    return paths;
}

} // end namespace 'co'
