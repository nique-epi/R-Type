#pragma once

#include <string>
#include "ConsoleChunk.hpp"

namespace rtype::server {

/** @brief What a LineEditor made of the bytes it read: the lines ended, and
 * what to write for the terminal to show the keys. */
struct LineEdit {
  ConsoleChunk chunk;
  std::string echo;
};

}  // namespace rtype::server
