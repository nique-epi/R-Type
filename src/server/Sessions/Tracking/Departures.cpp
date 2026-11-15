#include "Departures.hpp"
#include <array>
#include <cstdint>
#include <functional>
#include <optional>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "Endpoint.hpp"
#include "EventBus.hpp"
#include "EventField.hpp"
#include "LogEvent.hpp"
#include "LogLevel.hpp"
#include "Logger.hpp"
#include "PlayerDeparture.hpp"
#include "PlayerRoster.hpp"
#include "ProtocolConstants.hpp"
#include "ServerConstants.hpp"
#include "SessionConstants.hpp"

namespace rtype::server {

namespace {

/** @brief How the line of a departure names it: its event, and the words
 * between the number of the player and its address. */
struct DepartureWording {
  std::string_view event;
  std::string_view verb;
};

/** @returns The wording of a departure for @p cause. */
DepartureWording wordingOf(game::DepartureCause cause) {
  switch (cause) {
    case game::DepartureCause::Left:
      return {.event = PLAYER_LEFT_EVENT, .verb = " left from "};
    case game::DepartureCause::Expelled:
      return {.event = PLAYER_EXPELLED_EVENT,
              .verb = " expelled by the host from "};
    case game::DepartureCause::TimedOut:
      break;
  }
  return {.event = PLAYER_TIMED_OUT_EVENT,
          .verb = " timed out: nothing received from "};
}

/** @returns The field of the number of @p player. */
logging::EventField playerField(const ConnectedPlayer& player) {
  return {.name = PLAYER_FIELD, .value = std::int64_t{player.playerNumber}};
}

/** @returns The field of the address of @p player; it points into it. */
logging::EventField addressField(const ConnectedPlayer& player) {
  return {.name = ADDRESS_FIELD,
          .value = std::string_view{player.endpoint.address}};
}

/** @returns The field of the port of @p player. */
logging::EventField portField(const ConnectedPlayer& player) {
  return {.name = PORT_FIELD, .value = std::int64_t{player.endpoint.port}};
}

}  // namespace

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
  for (const network::Endpoint& player : expelledPlayers()) {
    close(player, game::DepartureCause::Expelled);
  }
  for (const network::Endpoint& player : countSilentTicks()) {
    close(player, game::DepartureCause::TimedOut);
  }
}

std::vector<ConnectedPlayer> Departures::takeDeparted() {
  return std::exchange(departed_, {});
}

std::vector<network::Endpoint> Departures::expelledPlayers() {
  std::vector<network::Endpoint> expelled;
  for (const ConnectedPlayer* const player : roster_.connected()) {
    if (player->expelled) {
      expelled.push_back(player->endpoint);
    }
  }
  return expelled;
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
  player.appliedInput.reset();
  std::ostringstream message;
  message << "player " << static_cast<unsigned int>(player.playerNumber)
          << " is away: nothing received from " << player.endpoint.address
          << " port " << player.endpoint.port << " for "
          << network::SESSION_TIMEOUT.count() << " s, seat kept for "
          << network::RECONNECTION_GRACE.count() << " s";
  const std::string text = message.str();
  const std::array fields{
      playerField(player), addressField(player), portField(player),
      logging::EventField{
          .name = SILENT_SECONDS_FIELD,
          .value = std::int64_t{network::SESSION_TIMEOUT.count()}},
      logging::EventField{
          .name = SEAT_KEPT_SECONDS_FIELD,
          .value = std::int64_t{network::RECONNECTION_GRACE.count()}}};
  logger_.event(logging::LogEvent{.level = logging::LogLevel::Info,
                                  .name = PLAYER_AWAY_EVENT,
                                  .message = text,
                                  .fields = fields});
}

void Departures::announce(const ConnectedPlayer& player,
                          game::DepartureCause cause) {
  logDeparture(player, cause);
  events_.publish(game::PlayerDeparture{.playerNumber = player.playerNumber,
                                        .cause = cause});
}

void Departures::logDeparture(const ConnectedPlayer& player,
                              game::DepartureCause cause) const {
  const DepartureWording wording = wordingOf(cause);
  const bool timedOut = cause == game::DepartureCause::TimedOut;
  std::ostringstream message;
  message << "player " << static_cast<unsigned int>(player.playerNumber)
          << wording.verb << player.endpoint.address << " port "
          << player.endpoint.port;
  if (timedOut) {
    message << " for " << network::SESSION_SILENCE_LIMIT.count() << " s";
  }
  const std::string text = message.str();
  const std::array fields{
      playerField(player), addressField(player), portField(player),
      logging::EventField{
          .name = SILENT_SECONDS_FIELD,
          .value = std::int64_t{network::SESSION_SILENCE_LIMIT.count()}}};
  logger_.event(logging::LogEvent{
      .level = logging::LogLevel::Info,
      .name = wording.event,
      .message = text,
      .fields = std::span{fields}.first(timedOut ? fields.size()
                                                 : fields.size() - 1)});
}

}  // namespace rtype::server
