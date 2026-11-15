#include "RouletteRoundAnnouncer.hpp"
#include <algorithm>
#include <cstddef>
#include <functional>
#include <utility>
#include <vector>
#include "CasinoConstants.hpp"
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "MessagePayload.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "RouletteRound.hpp"
#include "SessionDatagram.hpp"

namespace rtype::server {

namespace {

/** @returns Whether @p round says something @p sent did not: anything but the
 * ticks left. */
bool changedFrom(const network::RouletteRound& round,
                 const network::RouletteRound& sent) {
  network::RouletteRound sameClock = round;
  sameClock.ticksLeft = sent.ticksLeft;
  return sameClock != sent;
}

/** @returns Whether @p round lists @p player at the table. */
bool isListed(const network::RouletteRound& round,
              const ConnectedPlayer& player) {
  return std::ranges::find(round.seats, player.playerNumber,
                           &network::RouletteSeat::playerNumber) !=
         round.seats.end();
}

}  // namespace

RouletteRoundAnnouncer::RouletteRoundAnnouncer(
    PlayerRoster& roster, std::function<void(network::OutgoingDatagram)> send)
    : roster_(roster), send_(std::move(send)) {}

void RouletteRoundAnnouncer::onTick(const network::RouletteRound& round) {
  ++ticksSinceLastSend_;
  if (lastSent_.has_value() && !changedFrom(round, *lastSent_) &&
      ticksSinceLastSend_ < TICKS_PER_ROULETTE_ROUND) {
    return;
  }
  const std::vector<std::byte> payload = network::encodeMessage(round);
  for (ConnectedPlayer* const player : roster_.connected()) {
    if (isListed(round, *player)) {
      send_(sessionDatagram(*player, network::RouletteRound::TYPE, payload));
    }
  }
  lastSent_ = round;
  ticksSinceLastSend_ = 0;
}

void RouletteRoundAnnouncer::forget() {
  lastSent_.reset();
  ticksSinceLastSend_ = 0;
}

}  // namespace rtype::server
