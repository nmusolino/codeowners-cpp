#include <codeowners/type_utils.hpp>

#include <gtest/gtest.h>

#include <memory>

namespace co
{
namespace testing
{

TEST(type_utils, deleter_type)
{
    // This test simply confirms that certain usages of the `deleter_type`
    // template are compile-able.
    using T = int;
    deleter_type<T> deleter = [](T* v) { delete v; };

    // Confirm that deleter_type can be invoked on a pointer-to-T.
    deleter(static_cast<T*>(nullptr));

    // Confirm that deleter_type can be used with std::unique_ptr.
    std::unique_ptr<T, deleter_type<T>> p_i{new T{}, deleter};
}

struct X
{
    friend std::ostream& operator<<(std::ostream& os, const X&)
    {
        return os << "X";
    }
};

TEST(type_utils, is_streamable)
{
    static_assert(co::is_istreamable_v<int>);
    // Cannot stream into a const type.  For now, callers would be responsible
    // for using std::remove_cv_t
    static_assert(!co::is_istreamable_v<const int>);
    static_assert(co::is_istreamable_v<std::string>);

    static_assert(co::is_ostreamable_v<int>);
    static_assert(co::is_ostreamable_v<const int>);
    static_assert(co::is_ostreamable_v<std::string>);

    static_assert(!co::is_istreamable_v<X>);
    static_assert(co::is_ostreamable_v<X>);
}

} // end namespace 'testing'
} // end namespace 'co'
