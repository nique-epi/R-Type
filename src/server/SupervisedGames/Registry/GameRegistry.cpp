#include "GameRegistry.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <utility>
#include <vector>
#include "ConnectionRefused.hpp"
#include "ConnectionRequest.hpp"
#include "Endpoint.hpp"
#include "ExitedGame.hpp"
#include "GameLifecycle.hpp"
#include "GameOpening.hpp"
#include "GamePlace.hpp"
#include "GamePlacement.hpp"
#include "GameRules.hpp"
#include "GameState.hpp"
#include "GameStatus.hpp"
#include "GameVacancy.hpp"
#include "PlaceKind.hpp"
#include "ProcessExit.hpp"
#include "ProtocolConstants.hpp"
#include "SeatRequest.hpp"
#include "SequenceOrder.hpp"
#include "ServerConstants.hpp"
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
 * holds for players sent to it are taken, and only a game played by the
 * default rules takes any player. */
GameVacancy vacancyOf(const SupervisedGame& game) {
  return GameVacancy{
      .state = stateOf(game.lifecycle),
      .full = game.nicknames.size() + game.reservations.count() >=
              network::MAXIMUM_PLAYERS,
      .takesAnyPlayer = game.rules == network::defaultGameRules()};
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
 * to: the one that holds its seat, else the first played by the default rules
 * with a free seat; nothing when none fits. */
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

/** @returns The place of a player refused for @p reason. */
GamePlace refusalFor(network::RefusalReason reason) {
  return GamePlace{.kind = PlaceKind::Refuse, .port = 0, .refusal = reason};
}

/** @brief Gives the sender of @p request a seat of @p game, which has a free
 * seat or holds one for it.
 * @returns Its place in @p game. */
GamePlace seatIn(SupervisedGame& game, const SeatRequest& request) {
  game.reservations.reserve(request, game.nicknames);
  return placeIn(game);
}

/** @returns A seat in the game at @p index of @p games when there is one; else
 * Open while fewer than MAXIMUM_RUNNING_GAMES run, else a refusal with
 * ServerFull. */
GamePlace seatOrOpen(std::vector<SupervisedGame>& games,
                     std::optional<std::size_t> index,
                     const SeatRequest& request) {
  if (index.has_value()) {
    return seatIn(games[*index], request);
  }
  if (games.size() < MAXIMUM_RUNNING_GAMES) {
    return GamePlace{.kind = PlaceKind::Open, .port = 0};
  }
  return refusalFor(network::RefusalReason::ServerFull);
}

/** @returns Whether nobody but @p sender came to @p game, or holds a seat in
 * it: no player, and no seat held for another sender. */
bool onlyAwaits(const SupervisedGame& game, const network::Endpoint& sender) {
  return game.reservations.holds(sender) ||
         (game.nicknames.empty() && game.reservations.count() == 0);
}

/** @returns The index of the game of @p games that @p sender opened with a
 * NewGame, while it waits for players and nobody but its creator came;
 * nothing when none does. */
std::optional<std::size_t> gameCreatedBy(
    const std::vector<SupervisedGame>& games, const network::Endpoint& sender) {
  for (std::size_t index = 0; index < games.size(); ++index) {
    const SupervisedGame& game = games[index];
    if (game.creator == sender &&
        stateOf(game.lifecycle) == GameState::WaitingForPlayers &&
        onlyAwaits(game, sender)) {
      return index;
    }
  }
  return std::nullopt;
}

/** @returns Why the player of @p sender may not join @p game: GameInProgress
 * when it no longer waits for players, then GameFull when it has no free seat
 * and holds none for @p sender; nothing when it may. */
std::optional<network::RefusalReason> refusalToJoin(
    const SupervisedGame& game, const network::Endpoint& sender) {
  const GameVacancy vacancy = vacancyOf(game);
  if (vacancy.state != GameState::WaitingForPlayers) {
    return network::RefusalReason::GameInProgress;
  }
  if (vacancy.full && !game.reservations.holds(sender)) {
    return network::RefusalReason::GameFull;
  }
  return std::nullopt;
}

/** @returns The place of the player of @p request in the game of @p games
 * whose place is @p number, or why it is refused: GameNotFound when no game
 * holds that place, else as refusalToJoin() says. */
GamePlace placeInNumberedGame(std::vector<SupervisedGame>& games,
                              const SeatRequest& request,
                              std::uint16_t number) {
  const auto found = std::ranges::find(games, number, &SupervisedGame::slot);
  if (found == games.end()) {
    return refusalFor(network::RefusalReason::GameNotFound);
  }
  if (const std::optional<network::RefusalReason> refusal =
          refusalToJoin(*found, request.sender)) {
    return refusalFor(*refusal);
  }
  return seatIn(*found, request);
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
                                  .creator = std::move(opening.creator),
                                  .rules = std::move(opening.rules),
                                  .process = std::move(opening.process)});
  ++revision_;
}

SupervisedGame* GameRegistry::findByToken(std::uint64_t linkToken) {
  const auto found =
      std::ranges::find(games_, linkToken, &SupervisedGame::linkToken);
  return found == games_.end() ? nullptr : &*found;
}

bool GameRegistry::record(SupervisedGame& game, std::uint16_t sequence,
                          const network::GameStatus& status) {
  if (game.newestSequence.has_value() &&
      !network::isNewerSequence(sequence, *game.newestSequence)) {
    return false;
  }
  game.newestSequence = sequence;
  game.lifecycle = lifecycleOf(status.state);
  if (game.nicknames != status.nicknames) {
    game.nicknames = status.nicknames;
  }
  game.measures = status.measures;
  game.reservations.settle(game.nicknames);
  ++revision_;
  return true;
}

GamePlace GameRegistry::placeFor(const SeatRequest& request,
                                 const network::GameChoice& game) {
  switch (game.kind) {
    case network::GameChoiceKind::AnyGame:
      return seatOrOpen(games_, gameFor(games_, request.sender), request);
    case network::GameChoiceKind::NewGame:
      return seatOrOpen(games_, gameCreatedBy(games_, request.sender), request);
    case network::GameChoiceKind::ExistingGame:
      return placeInNumberedGame(games_, request, game.number);
  }
  return refusalFor(network::RefusalReason::GameNotFound);
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
  if (!exited.empty()) {
    ++revision_;
  }
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

const SupervisedGame* GameRegistry::findBySlot(std::uint16_t slot) const {
  const auto found = std::ranges::find(games_, slot, &SupervisedGame::slot);
  return found == games_.end() ? nullptr : &*found;
}

std::uint64_t GameRegistry::revision() const { return revision_; }

}  // namespace rtype::server
