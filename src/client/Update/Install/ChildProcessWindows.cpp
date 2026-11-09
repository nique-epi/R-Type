// clang-format off
#include <windows.h>
#include <shellapi.h>
// clang-format on
#include <filesystem>
#include <string>
#include "ChildProcess.hpp"
#include "LaunchCommand.hpp"

namespace rtype::client {

bool launchDetached(const LaunchCommand& command) {
  std::wstring parameters;
  for (const std::filesystem::path& argument : command.arguments) {
    if (!parameters.empty()) {
      parameters += L' ';
    }
    parameters += argument.native();
  }
  const std::wstring program = command.program.native();
  SHELLEXECUTEINFOW launch{};
  launch.cbSize = sizeof(launch);
  launch.fMask = SEE_MASK_NOASYNC;
  launch.lpFile = program.c_str();
  launch.lpParameters = parameters.c_str();
  launch.nShow = SW_SHOWNORMAL;
  return ShellExecuteExW(&launch) != FALSE;
}

}  // namespace rtype::client
