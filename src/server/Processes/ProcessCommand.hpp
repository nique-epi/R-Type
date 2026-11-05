#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace rtype::server {

/** @brief A program to start, and the arguments it gets after its own name. */
struct ProcessCommand {
  std::filesystem::path program;
  std::vector<std::string> arguments;
};

}  // namespace rtype::server
