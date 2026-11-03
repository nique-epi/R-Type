#include "Heartbeats.hpp"
#include <chrono>
#include <cstddef>
#include <functional>
#include <optional>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "Heartbeat.hpp"
#include "MessageOrigin.hpp"
#include "MessagePayload.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "RoundTripFigures.hpp"
#include "RoundTripWire.hpp"
#include "RoundTrips.hpp"
#include "SessionConstants.hpp"
#include "SessionDatagram.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

namespace {

/** @returns The round trip of each player of @p players measured, in the
 * order given; a player measured nothing with yet is left out. */
network::RoundTrips roundTripsOf(const ConnectedPlayers& players) {
  network::RoundTrips report;
  report.players.reserve(players.size());
  for (const ConnectedPlayer* const player : players) {
    const std::optional<network::RoundTripFigures> figures =
        player->roundTrip.figures();
    if (figures.has_value()) {
      report.players.push_back(
          network::playerRoundTripOf(player->playerNumber, *figures));
    }
  }
  return report;
}

}  // namespace

void addTimeSinceHeartbeat(PlayerRoster& roster, engine::Duration elapsed) {
  for (ConnectedPlayer* const player : roster.connected()) {
    player->sinceHeartbeat += elapsed;
  }
}

void sendHeartbeats(
    PlayerRoster& roster, engine::Duration now,
    const std::function<void(network::OutgoingDatagram)>& send) {
  const ConnectedPlayers players = roster.connected();
  std::optional<std::vector<std::byte>> report;
  for (ConnectedPlayer* const player : players) {
    if (player->sinceHeartbeat < TIME_BETWEEN_HEARTBEATS) {
      continue;
    }
    if (!report.has_value()) {
      report = network::encodeMessage(roundTripsOf(players));
    }
    player->sinceHeartbeat = engine::Duration::zero();
    send(sessionDatagram(*player, player->roundTrip.nextHeartbeat(now)));
    send(sessionDatagram(*player, network::RoundTrips::TYPE, *report));
  }
}

void readHeartbeat(PlayerRoster& roster, const network::MessageOrigin& origin,
                   const network::Heartbeat& heartbeat) {
  ConnectedPlayer* const player = roster.find(origin.sender);
  if (player != nullptr) {
    player->roundTrip.receive(heartbeat, origin.receivedAt);
  }
}

}  // namespace rtype::server
