#include "ServerMapScreenFixtures.hpp"
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "Endpoint.hpp"
#include "GameListSnapshot.hpp"
#include "GameRow.hpp"
#include "InterfaceColorConstants.hpp"
#include "ListedGameState.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "ServerCompatibility.hpp"
#include "ServerListPhase.hpp"
#include "ServerListSnapshot.hpp"
#include "ServerMapChoice.hpp"
#include "ServerMapScreenConstants.hpp"
#include "ServerMapScreenTestConstants.hpp"
#include "ServerRow.hpp"

namespace rtype::client::testing {

namespace {

ServerRow serverOf(std::int64_t serverId, std::string_view name) {
  ServerRow server;
  server.id = serverId;
  server.name = std::string{name};
  server.maximumPlayers = SERVER_SEATS;
  server.maximumPlayersPerGame = PLAYERS_PER_GAME;
  return server;
}

network::Endpoint gameEndpointAt(std::string_view address) {
  return network::Endpoint{.address = std::string{address},
                           .port = SERVER_GAME_PORT};
}

}  // namespace

std::vector<ServerRow> threeServers() {
  ServerRow lyon = serverOf(LYON_SERVER_ID, LYON_SERVER_NAME);
  lyon.endpoint = gameEndpointAt(LYON_SERVER_ADDRESS);
  lyon.ping = LYON_PING;
  ServerRow paris = serverOf(PARIS_SERVER_ID, PARIS_SERVER_NAME);
  paris.endpoint = gameEndpointAt(PARIS_SERVER_ADDRESS);
  paris.ping = PARIS_PING;
  ServerRow old = serverOf(OLD_STATION_ID, OLD_STATION_NAME);
  old.endpoint = gameEndpointAt(OLD_STATION_ADDRESS);
  old.compatibility = ServerCompatibility::UpdateNeeded;
  return {lyon, paris, old};
}

ServerListSnapshot readyList() {
  return ServerListSnapshot{.phase = ServerListPhase::Ready,
                            .servers = threeServers()};
}

ServerListSnapshot listWithLyonHolding(std::uint16_t players) {
  ServerListSnapshot list = readyList();
  list.servers.front().playerCount = players;
  return list;
}

ServerListSnapshot listWithNoSeatOfThisVersion() {
  ServerListSnapshot list = listWithLyonHolding(SERVER_SEATS);
  list.servers.at(PARIS_INDEX).playerCount = SERVER_SEATS;
  return list;
}

GameRow gameOf(std::uint16_t number, network::ListedGameState state) {
  return GameRow{
      .number = number,
      .name = std::string{number == WAITING_GAME_NUMBER ? WAITING_GAME_NAME
                                                        : PLAYING_GAME_NAME},
      .state = state,
      .playerCount = TWO_PILOTS,
      .maximumPlayers = PLAYERS_PER_GAME};
}

GameListSnapshot lyonGames() {
  return GameListSnapshot{
      .phase = ServerListPhase::Ready,
      .games = {gameOf(WAITING_GAME_NUMBER, network::ListedGameState::Waiting),
                gameOf(PLAYING_GAME_NUMBER, network::ListedGameState::InGame)},
      .difficulties = {},
      .server = {}};
}

sf::Vector2i parisOnTheWholeMap() {
  ServerMapChoice map;
  map.showServers(threeServers());
  return starPointsOf(map).at(PARIS_INDEX);
}

bool showsTheMiniMap(const InterfaceCanvasRecord& painting) {
  return std::ranges::any_of(
      painting.fills, [](const RecordedFill& fill) noexcept {
        return fill.color == INK &&
               MINIMAP_AREA.findIntersection(fill.area) == fill.area;
      });
}

GameListSnapshot waitingGames(std::size_t count) {
  GameListSnapshot games{.phase = ServerListPhase::Ready,
                         .games = {},
                         .difficulties = {},
                         .server = {}};
  for (std::size_t index = 0; index < count; ++index) {
    games.games.push_back(gameOf(static_cast<std::uint16_t>(index + 1),
                                 network::ListedGameState::Waiting));
  }
  return games;
}

}  // namespace rtype::client::testing
