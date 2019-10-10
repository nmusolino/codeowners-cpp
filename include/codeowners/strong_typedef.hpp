#pragma once

#include <codeowners/type_utils.hpp>

#include <boost/operators.hpp>
#include <type_traits>

namespace co
{

template <typename Tag, typename T>
struct strong_typedef
{
    using value_type = T;
    using param_type =
      std::conditional_t<std::is_fundamental_v<T>, T, const T&>;

    strong_typedef()
      : m_value{}
    {
        static_assert(std::is_default_constructible_v<T>);
    }

    explicit strong_typedef(const T& value)
      : m_value{value}
    {
    }
    explicit strong_typedef(T&& value) noexcept(
      std::is_nothrow_move_constructible_v<T>)
      : m_value{std::move(value)}
    {
    }

    const T& value() const& noexcept { return m_value; }
    const T&& value() const&& noexcept = delete;
    T& value() & noexcept { return m_value; }
    T&& value() && noexcept { return std::move(m_value); }

    explicit operator T&() noexcept { return value(); }
    explicit operator const T&() const noexcept { return value(); }

    friend void swap(strong_typedef& a,
                     strong_typedef& b) noexcept(std::is_nothrow_swappable_v<T>)
    {
        using std::swap;
        swap(static_cast<T&>(a), static_cast<T&>(b));
    }

private:
    T m_value;
};

template <typename StrongTypedef>
using underlying_type_t = typename StrongTypedef::value_type;

template <typename S>
struct equality_comparable : boost::equality_comparable<S>
{
    friend bool operator==(const S& a, const S& b)
    {
        return a.value() == b.value();
    }
};

template <typename S, typename U>
struct underlying_equality_comparable
  : equality_comparable<S>
  , boost::equality_comparable<S, U>
{
    friend bool operator==(const S& s, const U& u) { return s.value() == u; }
};

template <typename S>
struct less_than_comparable : boost::less_than_comparable<S>
{
    friend bool operator<(const S& a, const S& b)
    {
        return a.value() < b.value();
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
struct streamable
  : ostreamable<S>
  , istreamable<S>
{
};

} // end namespace 'co'