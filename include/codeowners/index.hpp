#pragma once

#include <codeowners/filesystem.hpp>
#include <codeowners/git_resources_fwd.hpp>
#include <codeowners/type_utils.hpp>

#include <boost/iterator/iterator_facade.hpp>

namespace co
{

class repository;

class index_iterator : public boost::iterator_facade<index_iterator, const fs::path,
                                                     boost::single_pass_traversal_tag>
{
    using iterator_facade = iterator_facade_;

public:
    index_iterator();

    reference dereference() const;
    bool equal(const index_iterator& other) const;
    void increment();

private:
    friend class index;
    index_iterator(const ::git_index*);
    bool is_end_iterator() const { return static_cast<bool>(m_ptr); }

private:
    std::shared_ptr<::git_index_iterator> m_ptr;
    fs::path m_path; // Re-use path buffer for efficiency.
};

class index
{
public:
    explicit index(const repository&);

    index_iterator begin() { return index_iterator{m_ptr.get()}; }
    index_iterator end() { return index_iterator{}; }

private:
    /// Return a pointer to the libgit2 index object.
    ::git_index* raw();
    const ::git_index* raw() const;

private:
    owning_ptr<::git_index> m_ptr;
};

} // end namespace 'co'