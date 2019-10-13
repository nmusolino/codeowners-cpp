#include "pattern_map.hpp"
#include <codeowners/ruleset.hpp>
#include <range/v3/view/move.hpp>
#include <range/v3/view/transform.hpp>

namespace co
{

namespace
{

using map_value_type = typename pattern_map<annotated_rule>::value_type;

std::unique_ptr<pattern_map<annotated_rule>>
make_rule_map(const std::vector<annotated_rule>& arules)
{
    auto make_pattern_pair = [](const annotated_rule& ar) -> map_value_type {
        return map_value_type{ar.rule.pattern, ar};
    };

    auto rule_pairs = arules | ranges::views::transform(make_pattern_pair);
    return std::make_unique<pattern_map<annotated_rule>>(rule_pairs.begin(),
                                                         rule_pairs.end());
}

std::unique_ptr<pattern_map<annotated_rule>>
make_rule_map(std::vector<annotated_rule>&& arules)
{
    auto make_pattern_pair = [](annotated_rule&& ar) -> map_value_type {
        pattern pat{ar.rule.pattern};
        return map_value_type{pat, std::move(ar)};
    };

    auto rule_pairs =
      ranges::views::move(arules) | ranges::views::transform(make_pattern_pair);
    return std::make_unique<pattern_map<annotated_rule>>(rule_pairs.begin(),
                                                         rule_pairs.end());
}

} // end anonymous namespace

ruleset::ruleset(const std::vector<annotated_rule>& rules)
  : m_rule_map{make_rule_map(rules)}
{
}

ruleset::ruleset(std::vector<annotated_rule>&& rules)
  : m_rule_map{make_rule_map(std::move(rules))}
{
}

ruleset::~ruleset() = default;

std::optional<annotated_rule>
ruleset::apply(const fs::path& path)
{
    const annotated_rule* arule = m_rule_map->get(path);

    std::optional<annotated_rule> result; // For NRVO
    return arule ? (result = *arule) : result;
}

} // end namespace 'co'