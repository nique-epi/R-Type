#pragma once

#include <string>

namespace rtype::server {

/** @brief The text of a waves file, and the name its errors give it: its
 * absolute path for a file read from disk. */
struct WaveFileText {
  std::string text;
  std::string source;
};

}  // namespace rtype::server
