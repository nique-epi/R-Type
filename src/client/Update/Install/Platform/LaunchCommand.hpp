#pragma once

#include <filesystem>
#include <vector>

namespace rtype::client {

/** @brief A program and its arguments. */
struct LaunchCommand {
  std::filesystem::path program;
  /** @brief Each argument, kept as a path so a folder keeps every character
   * on Windows, where the arguments are wide text. */
  std::vector<std::filesystem::path> arguments;
};

}  // namespace rtype::client
