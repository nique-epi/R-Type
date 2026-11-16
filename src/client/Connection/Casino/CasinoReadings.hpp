#pragma once

#include <optional>
#include "BetsReading.hpp"
#include "CasinoRoomReading.hpp"
#include "CasinoRoomsReading.hpp"
#include "OwnBetsReading.hpp"
#include "SeatReading.hpp"

namespace rtype::client {

/** @brief The newest answer of the master to each kind of ask of a
 * MasterCasinoLink; nothing before the first of its kind. */
struct CasinoReadings {
  std::optional<CasinoRoomsReading> rooms;
  std::optional<CasinoRoomReading> room;
  std::optional<SeatReading> seat;
  std::optional<BetsReading> bets;
  std::optional<OwnBetsReading> ownBets;
};

}  // namespace rtype::client
