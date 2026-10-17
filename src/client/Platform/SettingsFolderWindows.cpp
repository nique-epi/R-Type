// clang-format off
#include <windows.h>  // NOLINT(misc-include-cleaner)
#include <shlobj.h>   // NOLINT(misc-include-cleaner)
// clang-format on
#include <filesystem>
#include <memory>
#include <optional>
#include "PlatformConstants.hpp"
#include "SettingsFolder.hpp"

namespace rtype::client {

std::optional<std::filesystem::path> settingsFolder() {
  PWSTR reported = nullptr;
  // NOLINTNEXTLINE(misc-include-cleaner)
  const HRESULT result =
      SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &reported);
  const std::unique_ptr<wchar_t, decltype(&CoTaskMemFree)> owned(
      reported, &CoTaskMemFree);
  if (FAILED(result) || owned == nullptr) {
    return std::nullopt;
  }
  return std::filesystem::path(owned.get()) / WINDOWS_SETTINGS_FOLDER_NAME;
}

}  // namespace rtype::client
