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

index_iterator::index_iterator()
    : m_ptr{null_resource_ptr<::git_index_iterator>()}
{
}

index_iterator::index_iterator(const ::git_index* idx)
{
    auto it_ptr = make_resource_ptr<::git_index_iterator>(::git_index_iterator_new,
                                                          const_cast<::git_index*>(idx));
    m_ptr.reset(it_ptr.release(), resource_traits<::git_index_iterator>::deleter);
    increment();
}

const fs::path& index_iterator::dereference() const { return m_path; }

bool index_iterator::equal(const index_iterator& other) const
{
    if (is_end_iterator())
    {
        return other.is_end_iterator();
    }
    else
    {
        return !other.is_end_iterator() && m_ptr == other.m_ptr;
    }
}

void index_iterator::increment()
{
    const ::git_index_entry* entry;
    auto retval = ::git_index_iterator_next(&entry, m_ptr.get());
    std::cout << "next retval: " << retval << '\n';
    if (retval == 0)
    {
        m_path = entry->path;
        std::cout << "path: " << m_path << '\n';
    }
    else
    {
        std::cout << "iterator exhausted" << '\n';
        // Underlying iterator is exhausted.
        entry = nullptr;
        m_ptr.reset();
        m_path = "BAD_PATH_END_ITERATOR_DEREFERENCED";
    }
}

} // end namespace 'co'