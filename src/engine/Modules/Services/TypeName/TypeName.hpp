#pragma once

#include <string>
#include <typeindex>

namespace rtype::engine {

/**
 * @returns The name of @p type as written in the code, with its namespaces:
 * "rtype::engine::EventBus" for typeid(rtype::engine::EventBus), on GCC, Clang
 * and MSVC alike. GCC and Clang give a mangled name, decoded through the
 * Itanium C++ ABI (abi::__cxa_demangle in <cxxabi.h>); MSVC gives a readable
 * name with a leading keyword, removed. A name that cannot be decoded is
 * returned as the compiler gives it.
 */
[[nodiscard]] std::string typeName(std::type_index type);

}  // namespace rtype::engine
