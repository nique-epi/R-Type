#pragma once

#include <cstdint>
#include "RouletteDurations.hpp"

namespace rtype::master {

/** @brief The settings of the casino, as `master.cfg` gives them: the rooms,
 * numbered from network::FIRST_CASINO_ROOM, the players each seats, and the
 * phases of a round, the same in every room. */
struct CasinoSettings {
  std::uint8_t rooms{};
  std::uint8_t seats{};
  RouletteDurations durations;
};

}  // namespace rtype::master
