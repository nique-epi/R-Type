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
  const std::size_t now = countOf(seated, reservation.request.nickname);
  return now > reservation.namesakesBefore ? now - reservation.namesakesBefore
                                           : 0;
}

}  // namespace

bool SeatReservations::holds(const network::Endpoint& sender) const {
  return std::ranges::any_of(reservations_,
                             [&sender](const SeatReservation& reservation) {
                               return reservation.request.sender == sender;
                             });
}

void SeatReservations::reserve(const SeatRequest& request,
                               std::span<const std::string> seated) {
  if (holds(request.sender)) {
    return;
  }
  reservations_.push_back(
      SeatReservation{.request = request,
                      .namesakesBefore = countOf(seated, request.nickname),
                      .age = engine::Duration::zero()});
}

std::size_t SeatReservations::count() const { return reservations_.size(); }

void SeatReservations::settle(std::span<const std::string> seated) {
  if (reservations_.empty()) {
    return;
  }
  std::vector<std::string> ended;
  std::size_t kept = 0;
  for (std::size_t index = 0; index < reservations_.size(); ++index) {
    const std::string& nickname = reservations_[index].request.nickname;
    if (arrivalsFor(reservations_[index], seated) > countOf(ended, nickname)) {
      ended.push_back(nickname);
      continue;
    }
    if (kept != index) {
      reservations_[kept] = std::move(reservations_[index]);
    }
    ++kept;
  }
  reservations_.erase(reservations_.begin() + static_cast<std::ptrdiff_t>(kept),
                      reservations_.end());
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
