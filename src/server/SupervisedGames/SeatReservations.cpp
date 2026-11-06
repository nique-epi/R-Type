#include "SeatReservations.hpp"
#include <algorithm>
#include <cstddef>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "Endpoint.hpp"
#include "SeatRequest.hpp"
#include "SeatReservation.hpp"
#include "SupervisedGamesConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

namespace {

/** @returns How many of @p names are @p nickname. */
std::size_t countOf(std::span<const std::string> names,
                    const std::string& nickname) {
  return static_cast<std::size_t>(std::ranges::count(names, nickname));
}

/** @returns How many players of the nickname of @p reservation @p seated
 * names beyond those the game had when the seat was given. */
std::size_t arrivalsFor(const SeatReservation& reservation,
                        std::span<const std::string> seated) {
  const std::size_t now = countOf(seated, reservation.nickname);
  return now > reservation.namesakesBefore ? now - reservation.namesakesBefore
                                           : 0;
}

}  // namespace

bool SeatReservations::holds(const network::Endpoint& sender) const {
  return std::ranges::find(reservations_, sender, &SeatReservation::sender) !=
         reservations_.end();
}

void SeatReservations::reserve(const SeatRequest& request,
                               std::span<const std::string> seated) {
  if (holds(request.sender)) {
    return;
  }
  reservations_.push_back(
      SeatReservation{.sender = request.sender,
                      .nickname = request.nickname,
                      .namesakesBefore = countOf(seated, request.nickname),
                      .age = engine::Duration::zero()});
}

std::size_t SeatReservations::count() const { return reservations_.size(); }

void SeatReservations::settle(std::span<const std::string> seated) {
  std::vector<SeatReservation> kept;
  std::vector<std::string> ended;
  for (SeatReservation& reservation : reservations_) {
    if (arrivalsFor(reservation, seated) >
        countOf(ended, reservation.nickname)) {
      ended.push_back(reservation.nickname);
      continue;
    }
    kept.push_back(std::move(reservation));
  }
  reservations_ = std::move(kept);
}

void SeatReservations::age(engine::Duration elapsed) {
  for (SeatReservation& reservation : reservations_) {
    reservation.age += elapsed;
  }
  std::erase_if(reservations_, [](const SeatReservation& reservation) {
    return reservation.age >= SEAT_RESERVATION_TIMEOUT;
  });
}

}  // namespace rtype::server
