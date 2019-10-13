#pragma once

#include "codeowners/codeowners.hpp"

#include <vector>

namespace co
{

template <typename T> class pattern_map;

class ruleset
{
public:
    ruleset(const std::vector<annotated_rule>& rules);
    ruleset(std::vector<annotated_rule>&& rules);
    ~ruleset(); /* defaulted in cpp file */

    template <typename InputIt>
    ruleset(InputIt begin, InputIt end)
        : ruleset(std::vector<annotated_rule>(begin, end))
    {
    }

    std::optional<annotated_rule> apply(const fs::path& fs) const;

private:
    std::unique_ptr<pattern_map<annotated_rule>> m_rule_map;
};

} // end namespace 'co'