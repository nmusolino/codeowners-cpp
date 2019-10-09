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

} // end namespace 'testing'
} // end namespace 'co'
