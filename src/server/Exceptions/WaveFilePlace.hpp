#pragma once

#include <cstddef>
#include <string>

namespace rtype::server {

/**
 * @brief Where an error was found in a waves file: the file, the line and the
 * column it starts at, both counted from 1, and the dotted name of the key it
 * is about (`difficulty.normal.wave_pause_ms`), empty when it is about none.
 */
struct WaveFilePlace {
  std::string file;
  std::size_t line{};
  std::size_t column{};
  std::string key;
};

}  // namespace rtype::server
