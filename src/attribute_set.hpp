#pragma once

#include "codeowners/errors.hpp"
#include "codeowners/filesystem.hpp"
#include "codeowners/repository.hpp"
#include "codeowners/strong_typedef.hpp"

#include <boost/noncopyable.hpp>
#include <fstream>
#include <string>

namespace co
{

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

/**
 * The attribute_set class holds a collection of file pattern-attribute value
 * pairs, and provides member functions to obtain the attribute value for
 * any relative file path.
 *
 * This class is implemented by creating a temporary repository and writing
 * to a .gitattributes file, in order to take advantage of libgit2's
 * git attributes matching logic.
 */
class attribute_set : public boost::noncopyable
{
public:
    using value_type = std::string;

    class no_attribute_error : public error
    {
        using error::error;
    };

    attribute_set();
    attribute_set(const std::string& attribute_name);
    attribute_set(
      const std::vector<std::pair<pattern, value_type>>& associations);
    attribute_set(
      const std::string& attribute_name,
      const std::vector<std::pair<pattern, value_type>>& associations);

    /// Remove all pattern-value associations.
    void clear();

    /// Get the value of the attribute for the given relative path,
    /// based on patterns-attribute associations.  If no pattern
    /// matches the relative path, raises `no_attribute_error`.
    value_type get(const fs::path& relative_path) const;

    /// Get the value of the attribute for the given relative path,
    /// based on patterns-attribute associations.  If no pattern
    /// matches the relative path, returns the default `dflt`.
    value_type get(const fs::path& relative_path, const value_type& dflt) const;

    /// Get the value of the attribute for the given relative path,
    /// based on patterns-attribute associations.  If no pattern
    /// matches the relative path, returns an empty optional value.
    std::optional<value_type> get_optional(const fs::path& relative_path) const;

    /// Return the attribute name used within the internal implementation.
    const std::string& attribute_name() const& { return m_attribute_name; }

    /// Add a pattern-value association.
    void add_pattern(const pattern& pat, const value_type& value);

    void swap(attribute_set& other) noexcept;

private:
    void do_add_pattern(const pattern& pat, const value_type& value, bool sync);
    void do_sync();
private:
    static const char* const default_attribute_name;
    std::string m_attribute_name;
    temporary_directory_handle m_temp_dir;
    owning_ptr<::git_repository> m_repository_ptr;
    fs::path m_attributes_path;
    std::ofstream m_attributes_file; /// Output stream
};

} // end namespace 'co'
