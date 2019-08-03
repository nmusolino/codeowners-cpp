#pragma once

#include "codeowners/errors.hpp"
#include "codeowners/filesystem.hpp"
#include "codeowners/types.hpp"

#include <memory>
#include <optional>

struct git_repository; // forward
struct git_index;      // forward

namespace co
{

template <typename T>
using deleter_type = void (*)(T*);

template <typename T>
using resource_ptr = std::unique_ptr<T, deleter_type<T>>;

using repository_ptr = resource_ptr<::git_repository>;
using index_ptr = resource_ptr<::git_index>;

/// Class holding a file match pattern.  This is a strong typedef around a
/// string.
struct pattern
  : public strong_typedef<pattern, std::string>
  , equality_comparable<pattern>
  , streamable<pattern>
{
    using strong_typedef::strong_typedef;
};

/// Discover the repository root, starting at `location` and
/// searching parent directories.
std::optional<fs::path>
discover_repository(const fs::path& location, bool cross_filesystems = false);

/// Create a new git repository at the specified path.
repository_ptr
create_repository(const fs::path& path);


/**
 * The attribute_set class holds a collection of file pattern-attribute value
 * pairs, and provides member functions to obtain the attribute value for
 * any relative file path.
 *
 * This class is implemented by creating a temporary repository and writing
 * to a .gitattributes file, in order to take advantage of libgit2's
 * git attributes matching logic.
 */
class attribute_set
{
public:
    using value_type = std::string;

    class no_attribute_error : public error
    {
        using error::error;
    };

    attribute_set(const std::string& attribute_name);

    attribute_set(
      const std::string& attribute_name,
      const std::vector<std::pair<pattern, value_type>>& associations);

    /// Add a pattern-value association.
    void add_pattern(const pattern& pat, const value_type& value);

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

    /// Return the
    const std::string& attribute_name() const& { return m_attribute_name; }

private:
    ::git_repository* repo() { return m_repository_ptr.get(); }
    const ::git_repository* repo() const { return m_repository_ptr.get(); }

private:
    const std::string
      m_attribute_name; /// Attribute name used within .gitattributes file.
    temporary_directory_handle m_temp_dir; /// Temporary directory in which an
                                           /// empty git repository is created.
    repository_ptr m_repository_ptr; /// Pointer to repository data structure.
    fs::path
      m_attributes_path; /// Cached path to temporary .gitattributes file.
    std::ofstream m_attributes_file; /// Output stream
};

} // end namespace 'co'