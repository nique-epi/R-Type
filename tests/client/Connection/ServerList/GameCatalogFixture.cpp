#include "GameCatalogFixture.hpp"
#include <cstdint>
#include <string>
#include "GameCatalog.hpp"
#include "GameCatalogTestConstants.hpp"
#include "GameListing.hpp"
#include "GameRow.hpp"
#include "ListedGame.hpp"
#include "ListedGameState.hpp"
#include "RecordingMasterReads.hpp"
#include "ServerRow.hpp"

namespace rtype::client::testing {

ServerRow GameCatalogFixture::serverOf(std::int64_t id) {
  ServerRow server;
  server.id = id;
  server.maximumPlayersPerGame = PLAYERS_PER_GAME_OF_THE_SERVER;
  return server;
}

network::GameListing GameCatalogFixture::twoGames(
    network::ListedGameState first, network::ListedGameState second) {
  return network::GameListing{
      .games = {network::ListedGame{.number = FIRST_GAME_NUMBER,
                                    .name = std::string{FIRST_GAME_NAME},
                                    .state = first,
                                    .playerCount = TWO_PILOTS},
                network::ListedGame{.number = SECOND_GAME_NUMBER,
                                    .name = std::string{SECOND_GAME_NAME},
                                    .state = second,
                                    .playerCount = TWO_PILOTS}}};
}

GameRow GameCatalogFixture::gameIn(network::ListedGameState state) {
  return GameRow{.number = FIRST_GAME_NUMBER,
                 .name = std::string{FIRST_GAME_NAME},
                 .state = state,
                 .playerCount = TWO_PILOTS,
                 .maximumPlayers = PLAYERS_PER_GAME_OF_THE_SERVER};
}

GameCatalog& GameCatalogFixture::catalog() { return catalog_; }

RecordingMasterReads& GameCatalogFixture::master() { return master_; }

}  // namespace rtype::client::testing
