#include <codeowners/filesystem.hpp>

#include <range/v3/view/subrange.hpp>
#include <range/v3/view/transform.hpp>

#include <vector>

namespace co
{

ranges::subrange<fs::recursive_directory_iterator> make_file_range(const fs::path& start_point)
{
    return ranges::make_subrange(fs::recursive_directory_iterator{start_point},
                                 fs::recursive_directory_iterator{});
}

/**
 *  The `recursive_filter_iterator` class is an adapted iterator that can skip
 *  descending into directories based on a predicate.
 *
 *  The predicate in question is applied only to directories.  When the predicate
 *  returns true, the iterator traverses the contents of the directory as normal.
 *  When the predicate returns false, the directory will be skipped.
 *
 *  While this could be implemented as a filter on top of `recursive_filter_iterator`,
 *  it is more efficient to preemptively skip descending into directories, rather
 *  than iterating over everything and filtering out elements.  As an example, this
 *  could be used to skip recursive iteration through a large submodule's directory
 *  altogether, rather than iterating over (and subsequently dropping) every file in
 */
struct recursive_filter_iterator
    : public boost::iterator_adaptor<recursive_filter_iterator, fs::recursive_directory_iterator>
{
private:
    using base_type = base_type; // fs::recursive_directory_iterator
    using iterator_adaptor = iterator_adaptor_;

public:
    using predicate_type = std::function<bool(const fs::path&)>;

    recursive_filter_iterator() = default;

    recursive_filter_iterator(const fs::path& start_point, predicate_type predicate,
                              fs::symlink_option opt = fs::symlink_option::none)
        : iterator_adaptor{base_type{start_point, opt}}
        , m_predicate{std::move(predicate)}
    {
        assert(m_predicate);
        advance_to_next_unskipped();
    }

private:
    friend class boost::iterator_core_access;

    bool should_skip(const fs::directory_entry& dir_ent) const
    {
        return fs::is_directory(dir_ent) && !m_predicate(dir_ent);
    }

    void increment()
    {
        auto& base_it = base_reference();
        ++base_it;

        advance_to_next_unskipped();
    }

    void advance_to_next_unskipped()
    {
        auto& base_it = base_reference();
        while (base_it != base_type{} && should_skip(*base_it))
        {
            base_it.no_push(); // Do not descend into this directory.
            ++base_it;
        }
    }

private:
    predicate_type m_predicate;
};

template <typename Range>
ranges::subrange<recursive_filter_iterator> make_filtered_file_range(const fs::path& start_point,
                                                                     Range directories_to_skip)
{
    recursive_filter_iterator::predicate_type pred;

    if (ranges::empty(directories_to_skip))
    {
        pred = [](const fs::path&) { return true; };
    }
    else
    {
        // Ensure unary signature.
        auto canonical = [](const fs::path& p) { return fs::canonical(p); };

        // Make paths canonical at time of call.
        // TODO:  replace this with an unordered_set, using custom hash function.
        auto to_skip = ranges::views::all(directories_to_skip) | ranges::views::transform(canonical)
            | ranges::to<std::vector<fs::path>>();

        pred = [start_point, to_skip{std::move(to_skip)}](const fs::path& p) -> bool {
            auto is_equivalent_to_p = [p](const auto& elem) { return fs::equivalent(elem, p); };
            return std::none_of(to_skip.begin(), to_skip.end(), is_equivalent_to_p);
        };
    }
    assert(pred);
    return ranges::make_subrange(recursive_filter_iterator{start_point, std::move(pred)},
                                 recursive_filter_iterator{});
}

} /* end namespace 'co' */