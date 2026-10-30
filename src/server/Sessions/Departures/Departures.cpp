#include "Departures.hpp"
#include <functional>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "Endpoint.hpp"
#include "EventBus.hpp"
#include "Logger.hpp"
#include "PlayerDeparture.hpp"
#include "PlayerRoster.hpp"
#include "ProtocolConstants.hpp"
#include "SessionConstants.hpp"

namespace rtype::server {

Departures::Departures(PlayerRoster& roster, engine::EventBus& events,
                       std::function<void(const ConnectedPlayer&)> onClosing)
    : roster_(roster),
      events_(events),
      onClosing_(std::move(onClosing)),
      logger_(std::string{SESSIONS_LOGGER_NAME}) {}

void Departures::close(const network::Endpoint& player,
                       game::DepartureCause cause) {
  std::optional<ConnectedPlayer> departed = roster_.remove(player);
  if (!departed.has_value()) {
    return;
  }
  onClosing_(*departed);
  announce(*departed, cause);
  departed_.push_back(std::move(*departed));
}

void Departures::closeSilentSessions() {
  for (const network::Endpoint& player : countSilentTicks()) {
    close(player, game::DepartureCause::TimedOut);
  }
}

std::vector<ConnectedPlayer> Departures::takeDeparted() {
  return std::exchange(departed_, {});
}

std::vector<network::Endpoint> Departures::countSilentTicks() {
  std::vector<network::Endpoint> timedOut;
  for (ConnectedPlayer* const player : roster_.connected()) {
    if (player->ticksSinceReceived >= SESSION_CLOSING_TICKS) {
      timedOut.push_back(player->endpoint);
      continue;
    }
    ++player->ticksSinceReceived;
    if (!player->away && player->ticksSinceReceived >= SESSION_TIMEOUT_TICKS) {
      setAway(*player);
    }
  }
  return timedOut;
}

void Departures::setAway(ConnectedPlayer& player) const {
  player.away = true;
  player.newestInputNumber.reset();
  logger_.info("player ", static_cast<unsigned int>(player.playerNumber),
               " is away: nothing received from ", player.endpoint.address,
               " port ", player.endpoint.port, " for ",
               network::SESSION_TIMEOUT.count(), " s, seat kept for ",
               network::RECONNECTION_GRACE.count(), " s");
}

void Departures::announce(const ConnectedPlayer& player,
                          game::DepartureCause cause) {
  logDeparture(player, cause);
  events_.publish(game::PlayerDeparture{.playerNumber = player.playerNumber,
                                        .cause = cause});
}

void Departures::logDeparture(const ConnectedPlayer& player,
                              game::DepartureCause cause) const {
  const auto playerNumber = static_cast<unsigned int>(player.playerNumber);
  if (cause == game::DepartureCause::Left) {
    logger_.info("player ", playerNumber, " left from ",
                 player.endpoint.address, " port ", player.endpoint.port);
    return;
  }
  logger_.info("player ", playerNumber, " timed out: nothing received from ",
               player.endpoint.address, " port ", player.endpoint.port, " for ",
               network::SESSION_SILENCE_LIMIT.count(), " s");
}

}  // namespace rtype::server
