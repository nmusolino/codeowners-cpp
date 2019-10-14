#pragma once

#include <codeowners/errors.hpp>
#include <codeowners/strong_typedef.hpp>
#include <codeowners/type_utils.hpp>

#include <git2/buffer.h>
#include <git2/index.h>
#include <git2/repository.h>

#include <memory>
#include <optional>
#include <string>


namespace co
{

template <typename T>
struct resource_traits;

template <typename T>
using deleter_type = void (*)(T*);

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

template <typename T, typename F, typename... Args>
std::unique_ptr<T, deleter_type<T>> make_resource_ptr(F f, Args... args)
{
    using traits = resource_traits<T>;
    T* resource = nullptr;
    int error = f(&resource, std::forward<Args>(args)...);
    if (error)
    {
        using namespace std::string_literals;
        throw co::error{"Error while creating "s + traits::resource_name + ": error code "
                        + std::to_string(error)};
    }
    assert(resource);
    return std::unique_ptr<T, deleter_type<T>>(resource, resource_traits<T>::deleter);
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
    operator const ::git_buf&() { return m_buf; }

    ::git_buf* operator&() { return &m_buf; }
    const ::git_buf* operator&() const { return &m_buf; }

    void reserve(std::size_t size);

    const char* data() const { return m_buf.ptr; }
    std::size_t size() const { return m_buf.size; }
    bool empty() const { return size() == 0; }
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

} // end namespace 'co'