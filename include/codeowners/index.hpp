#pragma once

#include <codeowners/git_resources_fwd.hpp>
#include <codeowners/type_utils.hpp>

namespace co
{

class repository;

class index
{
public:
    explicit index(const repository&);

private:
    /// Return a pointer to the libgit2 index object.
    ::git_index* raw();
    const ::git_index* raw() const;

private:
    owning_ptr<::git_index> m_ptr;
};

} // end namespace 'co'