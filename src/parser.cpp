#include <codeowners/parser.hpp>

#include <boost/tokenizer.hpp>

#include <algorithm>
#include <iostream>

namespace co
{

bool is_all_whitespace(const std::string& s)
{
    // See note about conversion from `char` to `unsigned char`.
    // https://en.cppreference.com/w/cpp/string/byte/isspace
    return std::all_of(s.begin(), s.end(), [](unsigned char c) { return std::isspace(c); });
}

istream_tokenizer line_range(std::istream& is)
{
    using separator = boost::char_separator<char>;

    return istream_tokenizer{std::istreambuf_iterator<char>{is}, std::istreambuf_iterator<char>{},
                             separator{"\n", "", boost::keep_empty_tokens}};
}

std::optional<ownership_rule> parse_line(const std::string& line)
{
    if (line.empty() || is_all_whitespace(line))
    {
        return std::nullopt;
    }

    std::istringstream is{line};
    pattern pat;
    is >> pat;
    const std::string& pat_str = pat.value();
    if (pat_str.empty() or pat_str.at(0) == '#')
    {
        return std::nullopt;
    }

    return ownership_rule{
        std::move(pat),
        std::vector<owner>{std::istream_iterator<owner>{is}, std::istream_iterator<owner>{}}};
}

std::vector<annotated_rule> parse(const fs::path& path, const std::string& source_name)
{
    // Use `source_name` if provided.
    std::string eff_source_name = source_name.empty() ? path.string() : source_name;
    std::ifstream ifs{path.c_str()};
    return parse(ifs, eff_source_name);
}

std::vector<annotated_rule> parse(std::istream& is, const std::string& source_name)
{
    auto lines = line_range(is);
    return parse(lines.begin(), lines.end(), source_name);
}

}