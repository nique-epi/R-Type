#pragma once

#include <cstdint>
#include <string>
#include "RoulettePhase.hpp"

namespace rtype::network {

/** @brief One roulette room of the casino of the master, as the list of the
 * rooms shows it: `GET /api/casino/rooms`. */
struct CasinoRoom {
  /** @brief Its number, from FIRST_CASINO_ROOM. */
  std::uint8_t id{};
  std::string name;
  /** @brief The players it seats at most. */
  std::uint8_t seats{};
  /** @brief The players seated now. */
  std::uint8_t seated{};
  /** @brief The round its wheel plays now; every room plays the same. */
  std::uint64_t round{};
  RoulettePhase phase{RoulettePhase::Betting};

  bool operator==(const CasinoRoom&) const = default;
};

}  // namespace rtype::network
