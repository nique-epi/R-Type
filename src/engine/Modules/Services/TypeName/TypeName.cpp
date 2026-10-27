#include "TypeName.hpp"
#include <string>
#include <typeindex>

#if __has_include(<cxxabi.h>)
#include <cxxabi.h>
#include <cstdlib>
#include <memory>
#else
#include <string_view>
#include "TypeNameConstants.hpp"
#endif

namespace rtype::engine {

namespace {

#if __has_include(<cxxabi.h>)

std::string readableName(const char* compilerName) {
  int status = 0;
  const std::unique_ptr<char, decltype(&std::free)> decoded(
      abi::__cxa_demangle(compilerName, nullptr, nullptr, &status), &std::free);
  if (decoded == nullptr) {
    return compilerName;
  }
  return decoded.get();
}

#else

std::string readableName(const char* compilerName) {
  std::string_view name = compilerName;
  for (const std::string_view keyword : TYPE_KEYWORD_PREFIXES) {
    if (name.starts_with(keyword)) {
      name.remove_prefix(keyword.size());
      break;
    }
  }
  return std::string(name);
}

#endif

}  // namespace

std::string typeName(std::type_index type) { return readableName(type.name()); }

}  // namespace rtype::engine
