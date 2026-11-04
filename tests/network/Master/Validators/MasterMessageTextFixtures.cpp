#include "MasterMessageTextFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <string_view>
#include "HostedGame.hpp"
#include "HostedGameState.hpp"
#include "MasterMessageText.hpp"
#include "MasterMessageTextTestConstants.hpp"
#include "NetworkException.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerMeasures.hpp"
#include "ServerRegistration.hpp"

namespace rtype::network::testing {

namespace {

std::string withValueAt(const std::string& text, std::string_view pointer,
                        const nlohmann::json& value) {
  nlohmann::json document = nlohmann::json::parse(text);
  document[nlohmann::json::json_pointer(std::string{pointer})] = value;
  return document.dump();
}

std::string withoutValueAt(const std::string& text, std::string_view pointer) {
  nlohmann::json document = nlohmann::json::parse(text);
  const nlohmann::json::json_pointer member{std::string{pointer}};
  document.at(member.parent_pointer()).erase(member.back());
  return document.dump();
}

}  // namespace

ServerRegistration parisRegistration() {
  return ServerRegistration{
      .name = std::string{PARIS_NAME},
      .address = std::string{PARIS_ADDRESS},
      .udpPort = PARIS_UDP_PORT,
      .gameVersion = std::string{PARIS_GAME_VERSION},
      .protocolVersion = PARIS_PROTOCOL_VERSION,
      .maximumGames = PARIS_MAXIMUM_GAMES,
      .maximumPlayersPerGame = PARIS_MAXIMUM_PLAYERS_PER_GAME};
}

ServerHeartbeat parisHeartbeat() {
  return ServerHeartbeat{
      .games = {HostedGame{.number = WAITING_GAME_NUMBER,
                           .name = std::string{WAITING_GAME_NAME},
                           .state = HostedGameState::Waiting,
                           .difficulty = std::string{PARIS_DIFFICULTY},
                           .players = {std::string{FIRST_PILOT}}},
                HostedGame{.number = PLAYING_GAME_NUMBER,
                           .name = std::string{PLAYING_GAME_NAME},
                           .state = HostedGameState::Playing,
                           .difficulty = std::string{PARIS_DIFFICULTY},
                           .players = {std::string{SECOND_PILOT},
                                       std::string{THIRD_PILOT}}}},
      .measures = ServerMeasures{.tickP50Milliseconds = PARIS_TICK_P50,
                                 .tickP95Milliseconds = PARIS_TICK_P95,
                                 .receivedBytesPerSecond = PARIS_RECEIVED_BYTES,
                                 .sentBytesPerSecond = PARIS_SENT_BYTES,
                                 .lossPercent = PARIS_LOSS_PERCENT,
                                 .averagePingMilliseconds = std::nullopt}};
}

std::string registrationTextWith(std::string_view pointer,
                                 const nlohmann::json& value) {
  return withValueAt(registrationText(parisRegistration()), pointer, value);
}

std::string registrationTextWithout(std::string_view pointer) {
  return withoutValueAt(registrationText(parisRegistration()), pointer);
}

std::string heartbeatTextWith(std::string_view pointer,
                              const nlohmann::json& value) {
  return withValueAt(heartbeatText(parisHeartbeat()), pointer, value);
}

std::string heartbeatTextWithout(std::string_view pointer) {
  return withoutValueAt(heartbeatText(parisHeartbeat()), pointer);
}

std::string heartbeatTextOfGames(std::size_t gameCount) {
  ServerHeartbeat heartbeat;
  for (std::size_t index = 0; index < gameCount; ++index) {
    heartbeat.games.push_back(HostedGame{
        .number = static_cast<std::uint16_t>(index + 1),
        .name = std::string{GAME_NAME_PREFIX} + std::to_string(index + 1),
        .state = HostedGameState::Waiting,
        .difficulty = std::string{PARIS_DIFFICULTY},
        .players = {}});
  }
  return heartbeatText(heartbeat);
}

std::string registrationRefusal(std::string_view text) {
  try {
    static_cast<void>(registrationFromText(text));
  } catch (const InvalidMasterMessageException& refusal) {
    return refusal.what();
  }
  return {};
}

std::string createdServerRefusal(std::string_view text) {
  try {
    static_cast<void>(createdServerFromText(text));
  } catch (const InvalidMasterMessageException& refusal) {
    return refusal.what();
  }
  return {};
}

std::string heartbeatRefusal(std::string_view text) {
  try {
    static_cast<void>(heartbeatFromText(text));
  } catch (const InvalidMasterMessageException& refusal) {
    return refusal.what();
  }
  return {};
}

std::string quoted(std::string_view pointer) {
  return std::string{POINTER_QUOTE} + std::string{pointer} +
         std::string{POINTER_QUOTE};
}

}  // namespace rtype::network::testing
