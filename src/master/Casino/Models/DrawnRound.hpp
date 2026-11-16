#pragma once

#include <chrono>
#include <cstdint>
#include "RoomRound.hpp"

namespace rtype::master {

/** @brief A round of a room whose number was drawn, once its bets closed. */
struct DrawnRound {
  network::RoomRound place;
  std::uint8_t winningNumber{};
  std::chrono::sys_seconds drawnAt;
};

}  // namespace rtype::master
