#pragma once

#include <boost/operators.hpp>
#include <type_traits>

namespace co
{

template <typename Tag, typename T>
struct strong_typedef
{
    using type = T;

    template <typename = std::enable_if_t<std::is_default_constructible<T>::value>>
    explicit strong_typedef()
        : m_value {}
    {
    }

    explicit strong_typedef(const T& value)
        : m_value { value }
    {
    }
    explicit strong_typedef(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>)
        : m_value { std::move(value) }
    {
    }

    const T& value() const& noexcept { return m_value; }
    const T& value() const&& noexcept = delete;
    T& value() & noexcept { return m_value; }
    T& value() && noexcept = delete;

    explicit operator T&() noexcept { return value(); }
    explicit operator const T&() const noexcept { return value(); }

    friend void swap(strong_typedef& a, strong_typedef& b) noexcept(std::is_nothrow_swappable_v<T>)
    {
        using std::swap;
        swap(static_cast<T&>(a), static_cast<T&>(b));
    }

private:
    T m_value;
};

template <typename StrongTypedef>
using underlying_type_t = typename StrongTypedef::type;

template <typename S>
struct equality_comparable : boost::equality_comparable<S>
{
    friend bool operator==(const S& a, const S& b)
    {
        using T = underlying_type_t<S>;
        return static_cast<const T&>(a) == static_cast<const T&>(b);
    }
};

template <typename S, typename T>
struct underlying_equality_comparable : equality_comparable<S>,
                                        boost::equality_comparable<S, T>
{
    friend bool operator==(const S& a, const T& b)
    {
        return static_cast<const T&>(a) == b;
    }
};

template <typename S>
struct ostreamable
{
    friend std::ostream& operator<<(std::ostream& os, const S& s)
    {
        os << s.value();
        return os;
    }
};

template <typename S>
struct istreamable
{
    friend std::istream& operator>>(std::istream& is, S& s)
    {
        is >> s.value();
        return is;
    }
};

template <typename S>
struct streamable : ostreamable<S>, istreamable<S>
{
};

}  // end namespace 'co'