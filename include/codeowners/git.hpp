#pragma once

#include "codeowners/errors.hpp"
#include "codeowners/filesystem.hpp"

#include <boost/lexical_cast.hpp>
#include <memory>
#include <string>

struct git_repository;  // forward
struct git_index;  // forward

namespace co
{

template <typename T>
using deleter_type = void (*)(T*);

template <typename T>
using resource_ptr = std::unique_ptr<T, deleter_type<T>>;

using repository_ptr = resource_ptr<::git_repository>;
using index_ptr = resource_ptr<::git_index>;

repository_ptr create_repository(const fs::path& path);

class pattern_value_set
{
public:
    class no_attribute_value : public error
    {
        using error::error;
    };

    pattern_value_set(const std::string& attribute_name)
        : m_attribute_name { attribute_name }
        , m_repository_ptr { create_repository(m_temp_dir) }
        , m_attributes_file { attributes_path().string() }
    {
        assert(m_repository_ptr);
        assert(m_attributes_file.is_open());
    }

    template <typename T>
    void add_pattern(const std::string& pattern, const T& value)
    {
        _add_pattern(pattern, boost::lexical_cast<std::string>(value));
    }

    template <typename T>
    T get(const fs::path& relative_path) const
    {
        if (auto maybe_value_str = _get_value(relative_path))
        {
            return boost::lexical_cast<T>(*maybe_value_str);
        }
        using namespace std::string_literals;
        throw no_attribute_value { "No attribute value for: "s + relative_path.string() };
    }

    template <typename T>
    T get(const fs::path& relative_path, const T& dflt) const
    {
        if (auto maybe_value_str = _get_value(relative_path))
        {
            return boost::lexical_cast<T>(*maybe_value_str);
        }
        return dflt;
    }

    template <typename T>
    std::optional<T> get_optional(const fs::path& relative_path) const
    {
        if (auto maybe_value_str = _get_value(relative_path))
        {
            return boost::lexical_cast<T>(*maybe_value_str);
        }
        return std::nullopt;
    }

    const std::string& attribute_name() const& { return m_attribute_name; }

private:
    ::git_repository* repo() { return m_repository_ptr.get(); }
    const ::git_repository* repo() const { return m_repository_ptr.get(); }

    void _add_pattern(const std::string& pattern, const std::string& value);
    std::optional<std::string> _get_value(const fs::path& relative_path) const;

    fs::path attributes_path() const { return m_temp_dir / ".gitattributes"; }

private:
    const std::string m_attribute_name;
    temporary_directory_handle m_temp_dir;
    repository_ptr m_repository_ptr;
    std::ofstream m_attributes_file;
};

}  // end namespace 'co'