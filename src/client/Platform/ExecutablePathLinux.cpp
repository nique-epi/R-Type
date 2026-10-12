#include <filesystem>
#include <optional>
#include <system_error>
#include "ExecutablePath.hpp"
#include "PlatformConstants.hpp"

namespace rtype::client {

std::optional<std::filesystem::path> executablePath() {
  std::error_code error;
  std::filesystem::path executable =
      std::filesystem::read_symlink(LINUX_EXECUTABLE_LINK, error);
  if (error) {
    return std::nullopt;
  }
  return executable;
}

}  // namespace rtype::client
