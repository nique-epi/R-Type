#include "CasinoChannel.hpp"
#include <cstdint>
#include <optional>
#include <vector>
#include "BetsReading.hpp"
#include "CasinoReadings.hpp"
#include "CasinoRefusal.hpp"
#include "CasinoRoomListing.hpp"
#include "CasinoRoomState.hpp"
#include "IAccountSession.hpp"
#include "ICasinoLink.hpp"
#include "IWallet.hpp"
#include "RoomRound.hpp"
#include "RouletteBet.hpp"
#include "RoundBets.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

CasinoChannel::CasinoChannel(ICasinoLink& link, const IAccountSession& session,
                             IWallet& wallet)
    : link_(&link),
      watch_(link),
      seat_(link, session),
      desk_(link, session, wallet) {}

std::optional<network::CasinoRoomListing> CasinoChannel::rooms() const {
  const CasinoReadings readings = link_->readings();
  if (!readings.rooms.has_value()) {
    return std::nullopt;
  }
  return readings.rooms->listing;
}

void CasinoChannel::askRooms() { static_cast<void>(link_->askRooms()); }

void CasinoChannel::enter(std::uint8_t room) {
  if (watch_.room() == room) {
    return;
  }
  desk_.forget();
  watch_.watch(room);
  seat_.take(room);
}

void CasinoChannel::leave() {
  seat_.leave();
  watch_.stop();
  desk_.forget();
}

std::optional<network::CasinoRoomState> CasinoChannel::room() const {
  return watch_.shown();
}

std::optional<CasinoRefusal> CasinoChannel::seatRefusal() const {
  return seat_.refusal();
}

void CasinoChannel::placeBets(std::uint64_t round,
                              const std::vector<network::RouletteBet>& bets) {
  if (const std::optional<std::uint8_t> room = watch_.room()) {
    desk_.place(network::RoomRound{.room = *room, .round = round}, bets);
  }
}

std::optional<BetsReading> CasinoChannel::lastBets() const {
  return desk_.lastBets();
}

std::optional<network::RoundBets> CasinoChannel::ownBets(
    std::uint64_t round) const {
  return desk_.ownBets(round);
}

void CasinoChannel::update(engine::Duration elapsed) {
  const CasinoReadings readings = link_->readings();
  watch_.follow(readings.room, elapsed);
  seat_.follow(readings.seat, elapsed);
  desk_.follow(readings, watch_.shown());
}

}  // namespace rtype::client
