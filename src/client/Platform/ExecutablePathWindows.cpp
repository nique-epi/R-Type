#include <windows.h>
#include <filesystem>
#include <optional>
#include <string>
#include "ExecutablePath.hpp"
#include "PlatformConstants.hpp"

namespace rtype::client {

std::optional<std::filesystem::path> executablePath() {
  std::wstring reported(MAXIMUM_WINDOWS_PATH_LENGTH, L'\0');
  const DWORD length = GetModuleFileNameW(nullptr, reported.data(),
                                          static_cast<DWORD>(reported.size()));
  if (length == 0 || length == reported.size()) {
    return std::nullopt;
  }
  reported.resize(length);
  return std::filesystem::path(reported);
}

}  // namespace rtype::client
