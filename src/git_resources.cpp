#include <codeowners/errors.hpp>
#include <codeowners/filesystem.hpp>
#include <codeowners/git_resources.hpp>

#include <git2/attr.h>
#include <git2/errors.h>
#include <git2/global.h> /* git_libgit2_{init,shutdown} functions */
#include <git2/index.h>
#include <git2/pathspec.h>
#include <git2/repository.h>

#include <array>
#include <iostream>
#include <memory>
#include <string>

namespace co
{

struct libgit_handle
{
    libgit_handle() { ::git_libgit2_init(); }
    ~libgit_handle() { ::git_libgit2_shutdown(); }
};

namespace
{
/* Module-level global to ensure that init/shutdown functions are called. */
static libgit_handle handle;
} /* end anonymous namespace */

template <typename T>
struct resource_traits;

template <>
struct resource_traits<::git_repository>
{
    using value_type = ::git_repository;
    constexpr static deleter_type<value_type> deleter = ::git_repository_free;
    constexpr static const char* resource_name = "git_repository";
};

template <>
struct resource_traits<::git_index>
{
    using value_type = ::git_index;
    constexpr static deleter_type<value_type> deleter = ::git_index_free;
    constexpr static const char* resource_name = "git_index";
};

template <>
struct resource_traits<::git_pathspec>
{
    using value_type = ::git_pathspec;
    constexpr static deleter_type<value_type> git_pathspec =
      ::git_pathspec_free;
    constexpr static const char* resource_name = "git_pathspec";
};

template <typename T, typename F, typename... Args>
resource_ptr<T>
make_resource_ptr(F f, Args... args)
{
    using traits = resource_traits<T>;
    T* resource = nullptr;
    int error = f(&resource, std::forward<Args>(args)...);
    if (error)
    {
        using namespace std::string_literals;
        throw co::error{"Error while creating "s + traits::resource_name +
                        ": error code " + std::to_string(error)};
    }
    assert(resource);
    return resource_ptr<T>(resource, resource_traits<T>::deleter);
};

class git_buffer
{
public:
    git_buffer()
      : m_buf{empty_buf()}
    {
    }

    ~git_buffer() { reset(); }

    void reset()
    {
        ::git_buf_dispose(&m_buf);
        m_buf = empty_buf();
    }

    operator ::git_buf&() { return m_buf; }
    ::git_buf* buf_ptr() { return &m_buf; }

    const char* data() const { return m_buf.ptr; }
    std::size_t size() const { return m_buf.size; }
    std::size_t capacity() const
    {
        // If we allow construction from user-owned storage (e.g. a static
        // array), this function should take the max of m_buf.asize and
        // m_buf.size.
        return m_buf.asize;
    }
    std::string string() const { return std::string(data(), size()); }

private:
    static ::git_buf empty_buf() { return {nullptr, 0, 0}; }
    ::git_buf m_buf;
};

std::optional<fs::path>
discover_repository(const fs::path& location, bool across_fs)
{
    git_buffer buf;
    int err = ::git_repository_discover(
      buf.buf_ptr(), location.c_str(), across_fs, /* ceiling dirs */ nullptr);

    switch (err)
    {
        case 0:
        {
            assert(buf.data());
            return fs::canonical(fs::path{buf.data()});
        }
        case GIT_ENOTFOUND:
        {
            return std::nullopt;
        }
        default:
        {
            using namespace std::string_literals;
            throw error{"Error discovering git repository: "s +
                        location.c_str()};
        }
    }
    assert(false && "Unreachable");
}

repository_ptr
create_repository(const fs::path& path)
{
    return make_resource_ptr<::git_repository>(
      ::git_repository_init, path.c_str(), /*is_bare*/ false);
}

attribute_set::attribute_set(const std::string& attribute_name)
  : m_attribute_name{attribute_name}
  , m_repository_ptr{create_repository(m_temp_dir)}
  , m_attributes_path{m_temp_dir / ".gitattributes"}
  , m_attributes_file{m_attributes_path.string()}
{
    assert(m_repository_ptr);
    assert(m_attributes_file.is_open());
}

attribute_set::attribute_set(
  const std::string& attribute_name,
  const std::vector<std::pair<pattern, value_type>>& associations)
  : attribute_set{attribute_name}
{
    for (const auto& [pat, value] : associations)
    {
        add_pattern(pat, value);
    }
}

void
attribute_set::add_pattern(const pattern& pat, const value_type& value)
{
    // Flush so that file on disk reflects addition.
    m_attributes_file << pat << '\t' << attribute_name() << '=' << value << '\n'
                      << std::flush;
    ::git_attr_cache_flush(repo());
}

attribute_set::value_type
attribute_set::get(const fs::path& relative_path) const
{
    if (auto maybe_value_str = get_optional(relative_path))
    {
        return *maybe_value_str;
    }
    using namespace std::string_literals;
    throw attribute_set::no_attribute_error{"No attribute value for: "s +
                                            relative_path.string()};
}

attribute_set::value_type
attribute_set::get(const fs::path& relative_path,
                   const attribute_set::value_type& dflt) const
{
    if (auto maybe_value_str = get_optional(relative_path))
    {
        return *maybe_value_str;
    }
    return dflt;
}

std::optional<attribute_set::value_type>
attribute_set::get_optional(const fs::path& relative_path) const
{
    const char* value = nullptr;
    constexpr std::uint32_t flags = GIT_ATTR_CHECK_NO_SYSTEM;
    assert(repo());
    int retval = ::git_attr_get(&value,
                                const_cast<::git_repository*>(repo()),
                                flags,
                                relative_path.c_str(),
                                attribute_name().c_str());
    if (retval)
    {
        using namespace std::string_literals;
        throw co::error{"Error getting attribute value: "s +
                        relative_path.string()};
    }
    if (GIT_ATTR_UNSPECIFIED(value))
    {
        return std::nullopt;
    }
    assert(value);
    assert(GIT_ATTR_HAS_VALUE(value));
    return std::string{value};
}

} // end namespace 'co'
