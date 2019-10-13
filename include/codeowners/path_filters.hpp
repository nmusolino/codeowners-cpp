#include <codeowners/repository.hpp>

#include <iostream>

namespace co
{

// TODO: index_filter and ignored_filter structs.

struct recursive_filter_iterator
  : public boost::iterator_adaptor<recursive_filter_iterator,
                                   fs::recursive_directory_iterator>
{
private:
    using iterator_adaptor = iterator_adaptor_;

public:
    recursive_filter_iterator() = default;

    recursive_filter_iterator(fs::path filename_to_skip,
                              fs::path start_point,
                              fs::symlink_option opt = fs::symlink_option::none)
      : iterator_adaptor{base_type{start_point, opt}}
      , m_filename_to_skip{std::move(filename_to_skip)}
    {
    }

private:
    friend class boost::iterator_core_access;

    // Public for testing purposes.
    bool should_skip(const fs::path& p)
    {
        return p.filename() == m_filename_to_skip;
    }

    void increment()
    {
        auto& base_it = base_reference();
        ++base_it;

        // If the end has naturally been reached, simply return.
        if (base_it == base_type{})
        {
            return;
        }

        // Skip zero or one instances of `m_to_skip`.
        // A while loop should not be necessary, since there must be an
        // intervening directory entry with a different final component.
        const fs::directory_entry& dir_ent = *base_it;
        if (should_skip(dir_ent.path()))
        {
            if (fs::is_directory(dir_ent))
            {
                base_it.no_push(); // Do not descend into this directory.
            }
            ++base_it;
        }
    }

private:
    fs::path m_filename_to_skip;
};

struct filtered_file_range
{
private:
    fs::path m_filename_to_skip;
    fs::path m_start_point;

public:
    filtered_file_range(fs::path filename_to_skip, fs::path start_point)
      : m_filename_to_skip{std::move(filename_to_skip)}
      , m_start_point{std::move(start_point)}
    {
    }

    recursive_filter_iterator begin() const
    {
        return recursive_filter_iterator{m_filename_to_skip, m_start_point};
    }

    recursive_filter_iterator end() const
    {
        return recursive_filter_iterator{};
    }
};

} /* end namespace 'co' */