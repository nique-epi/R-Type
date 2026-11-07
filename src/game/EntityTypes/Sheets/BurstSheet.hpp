#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include "DataFilePlace.hpp"

namespace rtype::game {

/**
 * @brief The table `[gameplay.burst]` of the sheet of a missile: when it
 * bursts, into how many shards, and the sheet of the shards.
 */
struct BurstSheet {
  /** @brief Ticks the missile lives before it bursts, at least 1. */
  std::size_t afterTicks{};
  /** @brief How many shards fly out of it. */
  std::uint8_t count{};
  /** @brief The name of the sheet of the shards, as the file writes it. */
  std::string shard;
  /** @brief The number of that sheet; 0 until EntitySheets checked that it
   * exists and numbered it. */
  std::uint8_t shardNumber{};
  /** @brief Where the file names that sheet, for the error EntitySheets raises
   * when it names no sheet or a sheet of another side. */
  engine::DataFilePlace shardPlace;
};

}  // namespace rtype::game
