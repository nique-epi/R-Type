#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <vector>
#include "Endpoint.hpp"
#include "SeatRequest.hpp"
#include "SeatReservation.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief The seats of one game the supervisor gave players it sent there,
 * which the newest status of the game does not count yet: a player redirected
 * takes a handshake with the game before the game reports it. One per sender:
 * a sender that asks again keeps the seat it holds and takes no other.
 *
 * A seat ends when a status of the game names one player more of its
 * nickname than the game had when the seat was given, or once it is
 * SEAT_RESERVATION_TIMEOUT old, so a player that never arrives frees it.
 */
class SeatReservations {
 public:
  /** @returns Whether @p sender holds a seat. */
  [[nodiscard]] bool holds(const network::Endpoint& sender) const;

  /** @brief Gives the sender of @p request a seat, unless it holds one;
   * @p seated are the nicknames of the newest status of the game. */
  void reserve(const SeatRequest& request, std::span<const std::string> seated);

  /** @returns How many seats are held. */
  [[nodiscard]] std::size_t count() const;

  /** @brief Ends the seats of the players @p seated, the nicknames of a status
   * newer than every one taken before: for each nickname, as many seats as it
   * names players more than when they were given, the oldest first. */
  void settle(std::span<const std::string> seated);

  /** @brief Lets @p elapsed pass, and ends the seats that are
   * SEAT_RESERVATION_TIMEOUT old. */
  void age(engine::Duration elapsed);

 private:
  std::vector<SeatReservation> reservations_;
};

}  // namespace rtype::server
