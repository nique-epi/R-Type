#include "SupervisedGamesReport.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <ratio>
#include <span>
#include <string>
#include <vector>
#include "GameLifecycle.hpp"
#include "HostedGame.hpp"
#include "HostedGameState.hpp"
#include "ProtocolConstants.hpp"
#include "ServerConstants.hpp"
#include "ServerMeasures.hpp"
#include "ServerOccupancy.hpp"
#include "SupervisedGame.hpp"
#include "SupervisedGamesConstants.hpp"

namespace rtype::server {

namespace {

/** @returns How the master names the state of @p game: a game that starts
 * waits for players, and is Full once they take every seat. */
network::HostedGameState hostedStateOf(const SupervisedGame& game) {
  switch (game.lifecycle) {
    case GameLifecycle::Starting:
    case GameLifecycle::WaitingForPlayers:
      return game.nicknames.size() >= network::MAXIMUM_PLAYERS
                 ? network::HostedGameState::Full
                 : network::HostedGameState::Waiting;
    case GameLifecycle::InProgress:
      return network::HostedGameState::Playing;
    case GameLifecycle::Finished:
      return network::HostedGameState::Finished;
  }
  return network::HostedGameState::Waiting;
}

network::HostedGame hostedGameOf(const SupervisedGame& game) {
  return network::HostedGame{
      .number = game.slot,
      .name = std::string{GAME_NAME_PREFIX} + std::to_string(game.slot),
      .state = hostedStateOf(game),
      .difficulty = std::string{DEFAULT_DIFFICULTY},
      .players = game.nicknames};
}

/** @returns The higher of @p highest and @p measure, the one present when
 * only one is. */
std::optional<std::uint32_t> higherOf(std::optional<std::uint32_t> highest,
                                      std::optional<std::uint32_t> measure) {
  if (!measure.has_value()) {
    return highest;
  }
  if (!highest.has_value()) {
    return measure;
  }
  return std::max(*highest, *measure);
}

/** @returns @p microseconds in milliseconds, with their fraction. */
double millisecondsOf(double microseconds) {
  return std::chrono::duration<double, std::milli>(
             std::chrono::duration<double, std::micro>(microseconds))
      .count();
}

std::optional<double> millisecondsOf(std::optional<std::uint32_t> measure) {
  if (!measure.has_value()) {
    return std::nullopt;
  }
  return millisecondsOf(static_cast<double>(*measure));
}

/** @returns The mean round trip of the players of @p games, each game weighed
 * by its players; nothing when no game with players reports one. */
std::optional<double> weightedRoundTripOf(
    std::span<const SupervisedGame> games) {
  double totalMicroseconds = 0.0;
  std::size_t players = 0;
  for (const SupervisedGame& game : games) {
    const std::optional<std::uint32_t> roundTrip =
        game.measures.averageRoundTripMicroseconds;
    if (roundTrip.has_value() && !game.nicknames.empty()) {
      totalMicroseconds += static_cast<double>(*roundTrip) *
                           static_cast<double>(game.nicknames.size());
      players += game.nicknames.size();
    }
  }
  if (players == 0) {
    return std::nullopt;
  }
  return millisecondsOf(totalMicroseconds / static_cast<double>(players));
}

/** @returns The seats of @p game a new player may take: none once it is in
 * progress or finished. */
std::size_t freeSeatsOf(const SupervisedGame& game) {
  const bool takesPlayers = game.lifecycle == GameLifecycle::Starting ||
                            game.lifecycle == GameLifecycle::WaitingForPlayers;
  if (!takesPlayers || game.nicknames.size() >= network::MAXIMUM_PLAYERS) {
    return 0;
  }
  return network::MAXIMUM_PLAYERS - game.nicknames.size();
}

}  // namespace

std::vector<network::HostedGame> supervisedHostedGames(
    std::span<const SupervisedGame> games) {
  std::vector<network::HostedGame> hosted;
  hosted.reserve(games.size());
  for (const SupervisedGame& game : games) {
    hosted.push_back(hostedGameOf(game));
  }
  std::ranges::sort(hosted, {}, &network::HostedGame::number);
  return hosted;
}

network::ServerMeasures supervisedMeasures(
    std::span<const SupervisedGame> games) {
  std::optional<std::uint32_t> slowestMedian;
  std::optional<std::uint32_t> slowestNinetyFifthPercentile;
  for (const SupervisedGame& game : games) {
    slowestMedian =
        higherOf(slowestMedian, game.measures.tickMedianMicroseconds);
    slowestNinetyFifthPercentile =
        higherOf(slowestNinetyFifthPercentile,
                 game.measures.tickNinetyFifthPercentileMicroseconds);
  }
  network::ServerMeasures measures{};
  measures.tickP50Milliseconds = millisecondsOf(slowestMedian);
  measures.tickP95Milliseconds = millisecondsOf(slowestNinetyFifthPercentile);
  measures.averagePingMilliseconds = weightedRoundTripOf(games);
  return measures;
}

ServerOccupancy supervisedOccupancy(std::span<const SupervisedGame> games) {
  std::size_t players = 0;
  std::size_t freeSeats = 0;
  for (const SupervisedGame& game : games) {
    players += game.nicknames.size();
    freeSeats += freeSeatsOf(game);
  }
  const std::size_t emptySlots =
      MAXIMUM_RUNNING_GAMES - std::min(games.size(), MAXIMUM_RUNNING_GAMES);
  freeSeats += emptySlots * network::MAXIMUM_PLAYERS;
  return ServerOccupancy{
      .playerCount = static_cast<std::uint16_t>(players),
      .gameCount = static_cast<std::uint16_t>(MAXIMUM_RUNNING_GAMES),
      .freeSeatCount = static_cast<std::uint16_t>(freeSeats)};
}

}  // namespace rtype::server
