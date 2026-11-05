#include "GameResultKeeper.hpp"
#include <chrono>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "GameResult.hpp"
#include "GameSettings.hpp"
#include "GameState.hpp"
#include "IResultsArchive.hpp"
#include "LivesConstants.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server {

namespace {

/**
 * @returns What @p player leaves of the game. LifeKeeper is the only one to
 * lower ConnectedPlayer::livesWithoutShip, by one per ship lost, and a ship is
 * given that value, so it holds the lives left even while the player has a
 * ship.
 */
PlayerResult resultOf(const ConnectedPlayer& player) {
  return PlayerResult{.nickname = player.nickname,
                      .score = player.score,
                      .deaths = static_cast<std::uint32_t>(
                          game::STARTING_LIVES - player.livesWithoutShip)};
}

}  // namespace

GameResultKeeper::GameResultKeeper(PlayerRoster& roster,
                                   IResultsArchive& archive,
                                   const GameSettings& settings)
    : roster_(roster),
      archive_(archive),
      difficulty_(settings.difficulty),
      vanilla_(settings.vanilla) {}

void GameResultKeeper::onPlayerClosing(const ConnectedPlayer& departed) {
  if (state_ == GameState::InProgress) {
    departed_.push_back(resultOf(departed));
  }
}

void GameResultKeeper::onStateChanged(const GameStateChanged& change) {
  state_ = change.current;
  if (change.current == GameState::InProgress) {
    startCounting();
  } else if (change.previous == GameState::InProgress &&
             change.current == GameState::Finished) {
    recordTheGame();
  }
}

void GameResultKeeper::startCounting() {
  playersAtStart_ = static_cast<std::uint8_t>(roster_.connected().size());
}

void GameResultKeeper::recordTheGame() {
  std::vector<PlayerResult> players = std::exchange(departed_, {});
  for (const ConnectedPlayer* const player : roster_.connected()) {
    players.push_back(resultOf(*player));
  }
  if (players.empty()) {
    return;
  }
  archive_.record(
      GameResult{.difficulty = difficulty_,
                 .playerCount = playersAtStart_,
                 .endedAt = std::chrono::floor<std::chrono::seconds>(
                     std::chrono::system_clock::now()),
                 .players = std::move(players),
                 .vanilla = vanilla_});
}

}  // namespace rtype::server
