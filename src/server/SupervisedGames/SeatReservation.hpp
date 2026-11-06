#pragma once

#include <cstddef>
#include <string>
#include "Endpoint.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

/** @brief A seat of a game the supervisor gave a player, kept until the game
 * reports that player seated, or until SEAT_RESERVATION_TIMEOUT. */
struct SeatReservation {
  network::Endpoint sender;
  std::string nickname;
  /** @brief How many players of that nickname the game had when the seat was
   * given: one more in a status means the player sat down. */
  std::size_t namesakesBefore{};
  engine::Duration age{};
};

}  // namespace rtype::server
