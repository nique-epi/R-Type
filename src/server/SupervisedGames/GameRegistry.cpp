#include "GameRegistry.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <utility>
#include <vector>
#include "Endpoint.hpp"
#include "ExitedGame.hpp"
#include "GameLifecycle.hpp"
#include "GameOpening.hpp"
#include "GamePlace.hpp"
#include "GamePlacement.hpp"
#include "GameState.hpp"
#include "GameStatus.hpp"
#include "GameVacancy.hpp"
#include "MasterMessageConstants.hpp"
#include "PlaceKind.hpp"
#include "ProcessExit.hpp"
#include "ProtocolConstants.hpp"
#include "SeatRequest.hpp"
#include "SequenceOrder.hpp"
#include "ServerException.hpp"
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

/** @returns The state and the seats of @p game: its players and the seats it
 * holds for players sent to it are taken. */
GameVacancy vacancyOf(const SupervisedGame& game) {
  return GameVacancy{
      .state = stateOf(game.lifecycle),
      .full = game.nicknames.size() + game.reservations.count() >=
              network::MAXIMUM_PLAYERS};
}

/** @returns The index of the game of @p games that waits for players and
 * holds a seat for @p sender; nothing when none does. */
std::optional<std::size_t> gameHolding(const std::vector<SupervisedGame>& games,
                                       const network::Endpoint& sender) {
  for (std::size_t index = 0; index < games.size(); ++index) {
    if (stateOf(games[index].lifecycle) == GameState::WaitingForPlayers &&
        games[index].reservations.holds(sender)) {
      return index;
    }
  }
  return std::nullopt;
}

/** @returns The index of the game of @p games the player of @p sender goes
 * to: the one that holds its seat, else the first with a free seat; nothing
 * when none fits. */
std::optional<std::size_t> gameFor(const std::vector<SupervisedGame>& games,
                                   const network::Endpoint& sender) {
  if (const std::optional<std::size_t> held = gameHolding(games, sender)) {
    return held;
  }
  std::vector<GameVacancy> vacancies;
  vacancies.reserve(games.size());
  for (const SupervisedGame& game : games) {
    vacancies.push_back(vacancyOf(game));
  }
  return firstJoinableGame(vacancies);
}

/** @returns The lowest place none of @p games holds.
 * @throws NoFreeGameSlotException when they hold every place. */
std::uint16_t lowestFreeSlot(const std::vector<SupervisedGame>& games) {
  for (std::uint16_t slot = network::FIRST_HOSTED_GAME_NUMBER;
       slot < network::FIRST_HOSTED_GAME_NUMBER + MAXIMUM_RUNNING_GAMES;
       ++slot) {
    if (std::ranges::find(games, slot, &SupervisedGame::slot) == games.end()) {
      return slot;
    }
  }
  throw NoFreeGameSlotException(MAXIMUM_RUNNING_GAMES);
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
  const std::uint16_t slot = lowestFreeSlot(games_);
  games_.push_back(SupervisedGame{.number = opening.number,
                                  .slot = slot,
                                  .lifecycle = GameLifecycle::Starting,
                                  .nicknames = {},
                                  .measures = {},
                                  .port = opening.port,
                                  .linkToken = opening.linkToken,
                                  .newestSequence = std::nullopt,
                                  .reservations = {},
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
  game.nicknames = status.nicknames;
  game.measures = status.measures;
  game.reservations.settle(game.nicknames);
  return true;
}

GamePlace GameRegistry::placeForNewPlayer(const SeatRequest& request) {
  if (const std::optional<std::size_t> joinable =
          gameFor(games_, request.sender)) {
    SupervisedGame& game = games_[*joinable];
    game.reservations.reserve(request, game.nicknames);
    return placeIn(game);
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
    game.reservations.age(elapsed);
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
