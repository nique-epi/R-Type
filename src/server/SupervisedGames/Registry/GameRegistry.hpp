#pragma once

#include <cstdint>
#include <span>
#include <vector>
#include "ExitedGame.hpp"
#include "GameOpening.hpp"
#include "GamePlace.hpp"
#include "GameStatus.hpp"
#include "SupervisedGame.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief The games the supervisor runs, in the order they were started, each
 * as the supervisor knows it. It holds what the game processes reported, and
 * the handles of the processes; it sends nothing and starts nothing.
 */
class GameRegistry {
 public:
  /** @brief Adds the game of @p opening after the others, Starting. */
  void open(GameOpening opening);

  /** @returns The game that holds @p linkToken, nullptr when none does. */
  [[nodiscard]] SupervisedGame* findByToken(std::uint64_t linkToken);

  /**
   * @brief Takes @p status as the state of @p game when @p sequence is newer
   * than the newest status taken so far (16-bit serial number arithmetic),
   * and ignores it otherwise: a status that arrives late must not undo a newer
   * one.
   * @returns Whether it was taken.
   */
  bool record(SupervisedGame& game, std::uint16_t sequence,
              const network::GameStatus& status);

  /**
   * @returns Where a new player goes: the first game that waits for players
   * and has a free seat, a game still Starting counting as one that waits
   * (Wait, or Redirect once it reported); else Open while fewer than
   * MAXIMUM_RUNNING_GAMES run, else Full.
   */
  [[nodiscard]] GamePlace placeForNewPlayer() const;

  /**
   * @brief Removes the games whose process ended.
   * @returns Each of them, in the order they were started.
   */
  [[nodiscard]] std::vector<ExitedGame> collectExits();

  /** @brief Lets @p elapsed pass for the games still Starting, and kills those
   * that waited GAME_START_TIMEOUT: they leave the registry at the next
   * collectExits(). */
  void terminateStale(engine::Duration elapsed);

  /** @brief Kills every game. */
  void terminateAll();

  [[nodiscard]] std::span<const SupervisedGame> all() const;

 private:
  std::vector<SupervisedGame> games_;
};

}  // namespace rtype::server
