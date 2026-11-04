#include "HostedGames.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "GameInstance.hpp"
#include "GameState.hpp"
#include "HostedGame.hpp"
#include "HostedGameState.hpp"
#include "MasterMessageConstants.hpp"
#include "ProtocolConstants.hpp"
#include "ServerConstants.hpp"

namespace rtype::server {

namespace {

std::vector<std::string> nicknamesOf(GameInstance& game) {
  std::vector<std::string> nicknames;
  for (const ConnectedPlayer* const player : game.roster().connected()) {
    nicknames.push_back(player->nickname);
  }
  return nicknames;
}

}  // namespace

network::HostedGameState hostedStateOf(GameState state, std::size_t players) {
  switch (state) {
    case GameState::WaitingForPlayers:
      return players == network::MAXIMUM_PLAYERS
                 ? network::HostedGameState::Full
                 : network::HostedGameState::Waiting;
    case GameState::Starting:
      return network::HostedGameState::Starting;
    case GameState::InProgress:
      return network::HostedGameState::Playing;
    case GameState::Finished:
      return network::HostedGameState::Finished;
  }
  return network::HostedGameState::Finished;
}

std::vector<network::HostedGame> hostedGamesOf(
    std::span<const std::unique_ptr<GameInstance>> games) {
  std::vector<network::HostedGame> hosted;
  hosted.reserve(games.size());
  for (std::size_t index = 0; index < games.size(); ++index) {
    GameInstance& game = *games[index];
    std::vector<std::string> nicknames = nicknamesOf(game);
    const auto number =
        static_cast<std::uint16_t>(network::FIRST_HOSTED_GAME_NUMBER + index);
    hosted.push_back(network::HostedGame{
        .number = number,
        .name = std::string{GAME_NAME_PREFIX} + std::to_string(number),
        .state = hostedStateOf(game.state(), nicknames.size()),
        .difficulty = game.difficulty(),
        .players = std::move(nicknames)});
  }
  return hosted;
}

}  // namespace rtype::server
