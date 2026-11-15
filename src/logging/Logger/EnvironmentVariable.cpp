#include "EnvironmentVariable.hpp"
#include <cstdlib>
#include <optional>
#include <string>
#include <string_view>

#if defined(_WIN32)
#include <cstddef>
#include <memory>
#endif

namespace rtype::logging {

std::optional<std::string> readEnvironment(std::string_view name) {
  const std::string terminatedName(name);
#if defined(_WIN32)
  char* rawValue = nullptr;
  std::size_t length = 0;
  if (_dupenv_s(&rawValue, &length, terminatedName.c_str()) != 0) {
    return std::nullopt;
  }
  const std::unique_ptr<char, decltype(&std::free)> owned(rawValue, &std::free);
  if (owned == nullptr) {
    return std::nullopt;
  }
  return std::string(owned.get());
#else
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  const char* value = std::getenv(terminatedName.c_str());
  if (value == nullptr) {
    return std::nullopt;
  }
  return std::string(value);
#endif
}

}  // namespace rtype::logging
