#include "MasterMessageTextFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <string_view>
#include "FinishedGame.hpp"
#include "FinishedGamePlayer.hpp"
#include "GameListing.hpp"
#include "GlobalLeaderboard.hpp"
#include "GlobalLeaderboardRow.hpp"
#include "HostedGame.hpp"
#include "HostedGameState.hpp"
#include "ListedGame.hpp"
#include "ListedGameState.hpp"
#include "ListedServer.hpp"
#include "MasterMessageText.hpp"
#include "MasterMessageTextTestConstants.hpp"
#include "NetworkException.hpp"
#include "SampleListing.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerListing.hpp"
#include "ServerMeasures.hpp"
#include "ServerRegistration.hpp"
#include "ServerSample.hpp"
#include "ServerStatus.hpp"

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
      .maximumPlayersPerGame = PARIS_MAXIMUM_PLAYERS_PER_GAME,
      .probeSecret = PARIS_PROBE_SECRET};
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

GameListing parisGameListing() {
  return GameListing{
      .games = {ListedGame{.number = WAITING_GAME_NUMBER,
                           .name = std::string{WAITING_GAME_NAME},
                           .state = ListedGameState::Waiting,
                           .playerCount = WAITING_GAME_PLAYER_COUNT},
                ListedGame{.number = PLAYING_GAME_NUMBER,
                           .name = std::string{PLAYING_GAME_NAME},
                           .state = ListedGameState::InGame,
                           .playerCount = PLAYING_GAME_PLAYER_COUNT}}};
}

ServerListing parisServerListing() {
  ServerRegistration listed = parisRegistration();
  listed.probeSecret = {};
  return ServerListing{
      .servers = {ListedServer{.id = PARIS_ID,
                               .registration = listed,
                               .status = ServerStatus::Online,
                               .gameCount = LISTED_GAME_COUNT,
                               .playerCount = LISTED_PLAYER_COUNT}}};
}

SampleListing parisSampleListing() {
  return SampleListing{
      .samples = {ServerSample{.takenAt = SAMPLE_DATE,
                               .gameCount = LISTED_GAME_COUNT,
                               .playerCount = LISTED_PLAYER_COUNT,
                               .measures = ServerMeasures{
                                   .tickP50Milliseconds = std::nullopt,
                                   .tickP95Milliseconds = PARIS_TICK_P95,
                                   .receivedBytesPerSecond = std::nullopt,
                                   .sentBytesPerSecond = std::nullopt,
                                   .lossPercent = PARIS_LOSS_PERCENT,
                                   .averagePingMilliseconds = std::nullopt}}}};
}

std::string gameListingTextWith(std::string_view pointer,
                                const nlohmann::json& value) {
  return withValueAt(gameListingText(parisGameListing()), pointer, value);
}

std::string gameListingTextOfGames(std::size_t gameCount) {
  GameListing listing;
  for (std::size_t index = 0; index < gameCount; ++index) {
    listing.games.push_back(ListedGame{
        .number = static_cast<std::uint16_t>(index + 1),
        .name = std::string{GAME_NAME_PREFIX} + std::to_string(index + 1),
        .state = ListedGameState::Waiting,
        .playerCount = 0});
  }
  return gameListingText(listing);
}

std::string gameListingRefusal(std::string_view text) {
  try {
    static_cast<void>(gameListingFromText(text));
  } catch (const InvalidMasterMessageException& refusal) {
    return refusal.what();
  }
  return {};
}

std::string serverListingTextWith(std::string_view pointer,
                                  const nlohmann::json& value) {
  return withValueAt(serverListingText(parisServerListing()), pointer, value);
}

std::string serverListingTextOfServers(std::size_t serverCount) {
  ServerListing listing;
  for (std::size_t index = 0; index < serverCount; ++index) {
    ListedServer server = parisServerListing().servers.front();
    server.id = static_cast<std::int64_t>(index) + 1;
    listing.servers.push_back(server);
  }
  return serverListingText(listing);
}

std::string serverListingRefusal(std::string_view text) {
  try {
    static_cast<void>(serverListingFromText(text));
  } catch (const InvalidMasterMessageException& refusal) {
    return refusal.what();
  }
  return {};
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

FinishedGame parisFinishedGame() {
  return FinishedGame{
      .playedAt = FINISHED_GAME_DATE,
      .difficulty = std::string{PARIS_DIFFICULTY},
      .playerCount = PLAYING_GAME_PLAYER_COUNT,
      .vanilla = true,
      .players = {
          FinishedGamePlayer{.nickname = std::string{FIRST_PILOT},
                             .accountId = std::nullopt,
                             .score = FIRST_PILOT_SCORE},
          FinishedGamePlayer{.nickname = std::string{SECOND_PILOT},
                             .accountId = std::string{SECOND_PILOT_ACCOUNT},
                             .score = SECOND_PILOT_SCORE}}};
}

std::string finishedGameTextWith(std::string_view pointer,
                                 const nlohmann::json& value) {
  return withValueAt(finishedGameText(parisFinishedGame()), pointer, value);
}

std::string finishedGameTextOfPlayers(std::size_t playerCount) {
  FinishedGame game = parisFinishedGame();
  game.players.clear();
  for (std::size_t index = 0; index < playerCount; ++index) {
    game.players.push_back(
        FinishedGamePlayer{.nickname = std::string{FIVE_PILOTS.at(index)},
                           .accountId = std::nullopt,
                           .score = FIRST_PILOT_SCORE});
  }
  return finishedGameText(game);
}

GlobalLeaderboard parisLeaderboard() {
  return GlobalLeaderboard{
      .scores = {GlobalLeaderboardRow{.rank = FIRST_RANK,
                                      .nickname = std::string{FIRST_PILOT},
                                      .score = FIRST_PILOT_SCORE,
                                      .playedAt = FINISHED_GAME_DATE,
                                      .serverId = PARIS_ID,
                                      .serverName = std::string{PARIS_NAME}}},
      .player = GlobalLeaderboardRow{.rank = SECOND_RANK,
                                     .nickname = std::string{SECOND_PILOT},
                                     .score = SECOND_PILOT_SCORE,
                                     .playedAt = FINISHED_GAME_DATE,
                                     .serverId = PARIS_ID,
                                     .serverName = std::string{PARIS_NAME}}};
}

std::string finishedGameRefusal(std::string_view text) {
  try {
    static_cast<void>(finishedGameFromText(text));
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
