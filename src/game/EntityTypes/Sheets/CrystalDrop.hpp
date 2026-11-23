#pragma once

#include <cstdint>
#include <string>
#include "DataFilePlace.hpp"

namespace rtype::game {

/**
 * @brief One crystal of the key `drops` of the sheet of an enemy: the name of
 * its sheet, where the file names it, and the number of that sheet, 0 until
 * EntitySheets checked that it is a crystal and numbered it.
 */
struct CrystalDrop {
  std::string name;
  engine::DataFilePlace place;
  std::uint8_t number{0};
};

}  // namespace rtype::game
