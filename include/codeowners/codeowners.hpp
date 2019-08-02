#pragma once

#include "codeowners/types.hpp"

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

/// Class holding a code ownership rule, corresponding to a single record in
/// CODEOWNERS file.
struct ownership_rule
{
    std::string pattern;
    std::vector<owner> owners;
};

} // end namespace 'co'
