#include "CasinoSeatKeeper.hpp"
#include <cstdint>
#include <optional>
#include "CasinoRefusal.hpp"
#include "CasinoRoomConstants.hpp"
#include "CasinoSeat.hpp"
#include "IAccountSession.hpp"
#include "ICasinoLink.hpp"
#include "SeatReading.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

CasinoSeatKeeper::CasinoSeatKeeper(ICasinoLink& link,
                                   const IAccountSession& session)
    : link_(&link), session_(&session) {}

void CasinoSeatKeeper::take(std::uint8_t room) {
  room_ = room;
  refusal_.reset();
  ask(true);
}

void CasinoSeatKeeper::leave() {
  if (room_.has_value()) {
    ask(false);
  }
  room_.reset();
  refusal_.reset();
}

void CasinoSeatKeeper::follow(const std::optional<SeatReading>& reading,
                              engine::Duration elapsed) {
  if (reading.has_value() && reading->ask == ask_ && reading->seat.seated) {
    refusal_ = reading->refusal;
  }
  if (!room_.has_value()) {
    return;
  }
  sinceAsked_ += elapsed;
  if (sinceAsked_ >= network::CASINO_SEAT_RENEWAL_INTERVAL) {
    ask(true);
  }
}

std::optional<CasinoRefusal> CasinoSeatKeeper::refusal() const {
  return refusal_;
}

void CasinoSeatKeeper::ask(bool seated) {
  sinceAsked_ = engine::Duration::zero();
  if (!session_->signedIn() || !room_.has_value()) {
    return;
  }
  ask_ = link_->askSeat(session_->token(),
                        CasinoSeat{.room = *room_, .seated = seated});
}

}  // namespace rtype::client
