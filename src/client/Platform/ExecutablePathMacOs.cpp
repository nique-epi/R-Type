#include <mach-o/dyld.h>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <system_error>
#include "ExecutablePath.hpp"

namespace rtype::client {

std::optional<std::filesystem::path> executablePath() {
  std::uint32_t length = 0;
  _NSGetExecutablePath(nullptr, &length);
  std::string reported(length, '\0');
  if (_NSGetExecutablePath(reported.data(), &length) != 0) {
    return std::nullopt;
  }
  std::error_code error;
  std::filesystem::path executable =
      std::filesystem::canonical(reported.c_str(), error);
  if (error) {
    return std::nullopt;
  }
  return executable;
}

}  // namespace rtype::client
