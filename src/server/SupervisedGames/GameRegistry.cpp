#include "GameRegistry.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <utility>
#include <vector>
#include "ExitedGame.hpp"
#include "GameLifecycle.hpp"
#include "GameOpening.hpp"
#include "GamePlace.hpp"
#include "GamePlacement.hpp"
#include "GameState.hpp"
#include "GameStatus.hpp"
#include "GameVacancy.hpp"
#include "PlaceKind.hpp"
#include "ProcessExit.hpp"
#include "ProtocolConstants.hpp"
#include "SequenceOrder.hpp"
#include "SupervisedGame.hpp"
#include "SupervisedGamesConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

namespace {

/** @returns The state a game of @p lifecycle counts as when a place is looked
 * for: a game that starts waits for players. */
GameState stateOf(GameLifecycle lifecycle) {
  switch (lifecycle) {
    case GameLifecycle::Starting:
    case GameLifecycle::WaitingForPlayers:
      return GameState::WaitingForPlayers;
    case GameLifecycle::InProgress:
      return GameState::InProgress;
    case GameLifecycle::Finished:
      return GameState::Finished;
  }
  return GameState::Finished;
}

GameVacancy vacancyOf(const SupervisedGame& game) {
  return GameVacancy{.state = stateOf(game.lifecycle),
                     .full = game.playerCount >= network::MAXIMUM_PLAYERS};
}

/** @returns The place of a player in @p game, which has a free seat. */
GamePlace placeIn(const SupervisedGame& game) {
  return GamePlace{.kind = game.lifecycle == GameLifecycle::Starting
                               ? PlaceKind::Wait
                               : PlaceKind::Redirect,
                   .port = game.port};
}

}  // namespace

void GameRegistry::open(GameOpening opening) {
  games_.push_back(SupervisedGame{.number = opening.number,
                                  .lifecycle = GameLifecycle::Starting,
                                  .port = opening.port,
                                  .linkToken = opening.linkToken,
                                  .newestSequence = std::nullopt,
                                  .process = std::move(opening.process)});
}

SupervisedGame* GameRegistry::findByToken(std::uint64_t linkToken) {
  const auto found =
      std::ranges::find(games_, linkToken, &SupervisedGame::linkToken);
  return found == games_.end() ? nullptr : &*found;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
bool GameRegistry::record(SupervisedGame& game, std::uint16_t sequence,
                          const network::GameStatus& status) {
  if (game.newestSequence.has_value() &&
      !network::isNewerSequence(sequence, *game.newestSequence)) {
    return false;
  }
  game.newestSequence = sequence;
  game.lifecycle = lifecycleOf(status.state);
  game.playerCount = status.playerCount;
  return true;
}

GamePlace GameRegistry::placeForNewPlayer() const {
  std::vector<GameVacancy> vacancies;
  vacancies.reserve(games_.size());
  for (const SupervisedGame& game : games_) {
    vacancies.push_back(vacancyOf(game));
  }
  if (const std::optional<std::size_t> joinable =
          firstJoinableGame(vacancies)) {
    return placeIn(games_[*joinable]);
  }
  return GamePlace{.kind = games_.size() < MAXIMUM_RUNNING_GAMES
                               ? PlaceKind::Open
                               : PlaceKind::Full,
                   .port = 0};
}

std::vector<ExitedGame> GameRegistry::collectExits() {
  std::vector<ExitedGame> exited;
  std::erase_if(games_, [&exited](SupervisedGame& game) {
    const std::optional<ProcessExit> exit = game.process->exited();
    if (!exit.has_value()) {
      return false;
    }
    exited.push_back(
        ExitedGame{.number = game.number,
                   .lifecycle = game.lifecycle,
                   .port = game.port,
                   .linkToken = game.linkToken,
                   .exitCode = exit->code,
                   .crashed = exit->code != 0 ||
                              game.lifecycle == GameLifecycle::Starting});
    return true;
  });
  return exited;
}

void GameRegistry::terminateStale(engine::Duration elapsed) {
  for (SupervisedGame& game : games_) {
    if (game.lifecycle != GameLifecycle::Starting) {
      continue;
    }
    game.sinceLaunch += elapsed;
    if (game.sinceLaunch >= GAME_START_TIMEOUT) {
      game.process->terminate();
    }
  }
}

void GameRegistry::terminateAll() {
  for (SupervisedGame& game : games_) {
    game.process->terminate();
  }
}

std::span<const SupervisedGame> GameRegistry::all() const { return games_; }

}  // namespace rtype::server
