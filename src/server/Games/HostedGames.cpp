#include "HostedGames.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "GameInstance.hpp"
#include "GameState.hpp"
#include "Games.hpp"
#include "HostedGame.hpp"
#include "HostedGameState.hpp"
#include "MasterMessageConstants.hpp"

namespace rtype::server {

namespace {

network::HostedGame hostedGameOf(GameInstance& game, std::uint16_t number,
                                 std::string_view difficulty) {
  network::HostedGame hosted{.number = number,
                             .state = hostedStateOf(game.state()),
                             .difficulty = std::string{difficulty},
                             .players = {}};
  for (const ConnectedPlayer* const player : game.roster().connected()) {
    hosted.players.push_back(player->nickname);
  }
  return hosted;
}

}  // namespace

network::HostedGameState hostedStateOf(GameState state) {
  switch (state) {
    case GameState::WaitingForPlayers:
      return network::HostedGameState::Waiting;
    case GameState::Starting:
      return network::HostedGameState::Starting;
    case GameState::InProgress:
      return network::HostedGameState::Playing;
    case GameState::Finished:
      return network::HostedGameState::Finished;
  }
  return network::HostedGameState::Waiting;
}

std::vector<network::HostedGame> hostedGamesOf(const Games& games,
                                               std::string_view difficulty) {
  std::vector<network::HostedGame> hosted;
  hosted.reserve(games.all().size());
  std::uint16_t number = network::FIRST_HOSTED_GAME_NUMBER;
  for (const std::unique_ptr<GameInstance>& game : games.all()) {
    hosted.push_back(hostedGameOf(*game, number, difficulty));
    ++number;
  }
  return hosted;
}

}  // namespace rtype::server
