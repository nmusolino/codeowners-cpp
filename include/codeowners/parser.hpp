#pragma once

#include <codeowners/codeowners.hpp>

#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/subrange.hpp>
#include <range/v3/view/transform.hpp>

#include <iosfwd>
#include <optional>
#include <vector>

namespace co
{

std::optional<ownership_rule> parse_line(const std::string& line);

std::vector<annotated_rule> parse(std::istream& is, const std::string& source_name = "");

std::vector<annotated_rule> parse(const fs::path& source, const std::string& source_name = "");

template <typename InputIt>
std::vector<annotated_rule> parse(InputIt begin, InputIt end, const std::string& source_name = "")
{
    auto make_annotated_rule = [&source_name](const std::pair<std::size_t, ownership_rule>& pair) {
        const auto& [i, rule] = pair;
        // Use `i+1`, since line numbers conventionally begin at 1.
        return annotated_rule{rule_source{source_name, static_cast<std::int32_t>(i + 1)}, rule};
    };

    auto input = ranges::make_subrange(begin, end);
    // We need to perform the line numbering before filtering, which makes the
    // filtering and transforming slightly more awkward.
    // clang-format off
    return input
           | ranges::views::transform(parse_line)
           | ranges::views::enumerate
           | ranges::views::filter([](const auto& p) { return p.second.has_value(); })
           | ranges::views::transform([](const auto& p) { return std::make_pair(p.first, *p.second); })
           | ranges::views::transform(make_annotated_rule)
           | ranges::to<std::vector<annotated_rule>>();
    // clang-format on
}

using istream_tokenizer
    = boost::tokenizer<boost::char_separator<char>, std::istreambuf_iterator<char>>;

istream_tokenizer line_range(std::istream& is);

} // end namespace 'co'