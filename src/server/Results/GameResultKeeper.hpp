#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "GameResult.hpp"
#include "GameSettings.hpp"
#include "GameState.hpp"
#include "GameStateChanged.hpp"
#include "IResultsArchive.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server {

/**
 * @brief Follows a game from its start to its end and records what it leaves:
 * a row per player connected when it started, as that player was when it left
 * or when the game ended.
 *
 * It reads the score at the tick of the end, before the bus delivers anything
 * more, so a player finds in the leaderboard the score GameEnded told them. A
 * game with nobody left to record leaves nothing. A game the server stops in
 * the middle has no end, so it leaves nothing either.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class GameResultKeeper {
 public:
  /**
   * @param roster The players of the game; it must outlive the keeper.
   * @param archive Takes the result of each game; it must outlive the keeper.
   * @param settings The settings of the game: its results are filed under its
   * difficulty, and say whether it is vanilla.
   */
  GameResultKeeper(PlayerRoster& roster, IResultsArchive& archive,
                   const GameSettings& settings);

  /** @brief Keeps what @p departed leaves when the game is in progress, as the
   * latest onStateChanged() says. Outside a game it keeps
   * nothing, so players coming and going in the waiting room cost no memory. */
  void onPlayerClosing(const ConnectedPlayer& departed);

  /**
   * @brief Follows the game from one state to the next: it counts the players
   * when the game starts, and records the game when it finishes.
   */
  void onStateChanged(const GameStateChanged& change);

 private:
  void startCounting();
  void recordTheGame();

  PlayerRoster& roster_;
  IResultsArchive& archive_;
  std::string difficulty_;
  bool vanilla_{false};
  GameState state_{GameState::WaitingForPlayers};
  std::uint8_t playersAtStart_{0};
  std::vector<PlayerResult> departed_;
};

}  // namespace rtype::server
