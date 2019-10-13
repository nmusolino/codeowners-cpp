#pragma once

#include <iostream>
#include <memory>

namespace co
{

template <typename T> using deleter_type = void (*)(T*);

template <typename T> using owning_ptr = std::unique_ptr<T, deleter_type<T>>;

template <typename T, typename = void> struct is_ostreamable : std::false_type
{
};

template <class T>
struct is_ostreamable<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>>
    : std::true_type
{
};

template <typename T> constexpr bool is_ostreamable_v = is_ostreamable<T>::value;

template <typename T, typename = void> struct is_istreamable : std::false_type
{
};

template <class T>
struct is_istreamable<T, std::void_t<decltype(std::declval<std::istream&>() >> std::declval<T&>())>>
    : std::true_type
{
};

template <typename T> constexpr bool is_istreamable_v = is_istreamable<T>::value;

} // end namespace 'co'