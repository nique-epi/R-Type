#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include "DataFilePlace.hpp"

namespace rtype::game {

/**
 * @brief The table `[gameplay.gun]` of a sheet, its durations turned into
 * whole simulation ticks, the nearest ones.
 */
struct GunSheet {
  /** @brief Ticks between two shots, at least 1. */
  std::size_t intervalTicks{};
  /** @brief Ticks before the first shot: 0 for a gun ready at once. */
  std::size_t firstShotTicks{};
  /** @brief The name of the sheet of what it fires, as the file writes it. */
  std::string projectile;
  /** @brief The number of that sheet; 0 until EntitySheets checked that it
   * exists and numbered it. */
  std::uint8_t projectileNumber{};
  /** @brief Where the file names that sheet, for the error EntitySheets raises
   * when it names no sheet or a sheet of the wrong side. */
  engine::DataFilePlace projectilePlace;
};

}  // namespace rtype::game
