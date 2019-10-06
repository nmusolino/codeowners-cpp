#include "attribute_set.hpp"
#include "git_resources.hpp"

#include <git2/attr.h>
#include <git2/repository.h>

namespace co
{

/* static */
const char* const attribute_set::default_attribute_name = "codeowners_attr";

attribute_set::attribute_set()
  : attribute_set(default_attribute_name)
{
}

attribute_set::attribute_set(const std::string& attribute_name)
  : m_attribute_name{attribute_name}
  , m_temp_dir{}
  , m_repository_ptr{make_resource_ptr<::git_repository>(::git_repository_init,
                                                         m_temp_dir.c_str(),
                                                         /*bare*/ false)}
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
        do_add_pattern(pat, value, /*sync*/ false);
    }
    do_sync();
}

attribute_set::attribute_set(
  const std::vector<std::pair<pattern, value_type>>& associations)
  : attribute_set(default_attribute_name, associations)
{
}

void
attribute_set::add_pattern(const pattern& pat, const value_type& value)
{
    do_add_pattern(pat, value, /*sync*/ true);
}

void
attribute_set::do_add_pattern(const pattern& pat,
                              const value_type& value,
                              bool sync)
{
    m_attributes_file << pat << '\t' << attribute_name() << '=' << value
                      << '\n';
    if (sync)
    {
        do_sync();
    }
}

void
attribute_set::do_sync()
{
    // Flush so that file on disk reflects any recent addition.
    m_attributes_file << std::flush;
    ::git_attr_cache_flush(m_repository_ptr.get());
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
    int retval =
      ::git_attr_get(&value,
                     const_cast<::git_repository*>(m_repository_ptr.get()),
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

}