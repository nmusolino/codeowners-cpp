#pragma once

#include <memory>

namespace co
{

template <typename T>
using deleter_type = void (*)(T*);

template <typename T>
using owning_ptr = std::unique_ptr<T, deleter_type<T>>;

} // end namespace 'co'