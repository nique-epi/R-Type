#pragma once

#include <cstddef>
#include <string>

namespace rtype::engine {

/**
 * @brief Where an error was found in a data file, a waves file or an entity
 * sheet: the file, the line and the column it starts at, both counted from 1,
 * and the dotted name of the key it is about
 * (`difficulty.normal.wave_pause_ms`), empty when it is about none.
 */
struct DataFilePlace {
  std::string file;
  std::size_t line{};
  std::size_t column{};
  std::string key;
};

}  // namespace rtype::engine
