#include <codeowners/index.hpp>

#include <codeowners/repository.hpp>

#include "git_resources.hpp"

#include <git2/index.h>
#include <git2/repository.h>

namespace co
{

index::index(const repository& repo)
    : m_ptr{make_resource_ptr<::git_index>(::git_repository_index,
                                           const_cast<::git_repository*>(repo.raw()))}
{
}

} // end namespace 'co'