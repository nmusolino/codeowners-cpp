#pragma once

#include "codeowners/filesystem.hpp"
#include "codeowners/strong_typedef.hpp"

#include <memory>
#include <string>
#include <vector>

namespace co
{

/// Class that identifies a code owner.  This is a strong typedef around a
/// string.
struct owner
  : public strong_typedef<owner, std::string>
  , equality_comparable<owner>
  , streamable<owner>
{
    using strong_typedef::strong_typedef;
};

/// Class holding a file match pattern.  This is a strong typedef around a
/// string.
struct pattern
  : public strong_typedef<pattern, std::string>
  , equality_comparable<pattern>
  , less_than_comparable<pattern>
  , streamable<pattern>
{
    using strong_typedef::strong_typedef;
};

/// Class holding a code ownership rule, corresponding to a single record in
/// CODEOWNERS file.
struct ownership_rule
{
    pattern pattern;
    std::vector<owner> owners;

    friend bool operator==(const ownership_rule& lhs, const ownership_rule& rhs)
    {
        return std::tie(lhs.pattern, lhs.owners) ==
               std::tie(rhs.pattern, rhs.owners);
    }

    friend std::ostream& operator<<(std::ostream& os,
                                    const ownership_rule& rule)
    {
        os << rule.pattern << "    ";
        for (const auto& owner : rule.owners)
        {
            os << owner << " ";
        }
        return os;
    }
};

struct rule_source
{
    std::string filename;
    std::int32_t line;

    friend bool operator==(const rule_source& lhs, const rule_source& rhs)
    {
        return std::tie(lhs.filename, lhs.line) ==
               std::tie(rhs.filename, rhs.line);
    }

    friend std::ostream& operator<<(std::ostream& os, const rule_source& src)
    {
        return os << src.filename << ":" << src.line;
    }
};

struct annotated_rule
{
    rule_source source;
    ownership_rule rule;

    friend bool operator==(const annotated_rule& lhs, const annotated_rule& rhs)
    {
        return std::tie(lhs.source, lhs.rule) == std::tie(rhs.source, rhs.rule);
    }

    friend std::ostream& operator<<(std::ostream& os,
                                    const annotated_rule& ann_rule)
    {
        return os << ann_rule.source << ":    " << ann_rule.rule;
    }
};

} // end namespace 'co'
