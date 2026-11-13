#pragma once

#include <optional>
#include <string>
#include <vector>
#include "LevelBosses.hpp"
#include "LevelDirectorParts.hpp"
#include "LevelProgress.hpp"
#include "Logger.hpp"
#include "LuauLevelRun.hpp"
#include "LuauLevels.hpp"

namespace rtype::server {

/**
 * @returns The stages of the level @p level among @p levels.
 * @throws UnknownLevelException, naming the levels the scripts registered,
 * when none of them is @p level.
 */
[[nodiscard]] std::vector<int> stagesOfLevel(const game::LuauLevels& levels,
                                             const std::string& level);

/**
 * @brief Directs the level a game plays in one world: it steps the stages
 * its script registered (see game::LuauLevelRun), scrolls the level, and
 * tells the game what the players must see of it.
 *
 * Each onTick() lets the bosses the level announced enter once their warning
 * is over (see LevelBosses), steps the stage due, then scrolls the level by
 * one tick unless its script paused the scroll. A game::LevelChange is
 * published on the bus of the game at the first tick, then each time the biome
 * changes, the scroll stops or starts again, or the level restarts from a
 * checkpoint.
 *
 * The level is cleared once its last stage returned and no enemy is left in
 * the world. A stage that fails makes the level failed, for good, which the
 * journal says once. restartFromCheckpoint() puts the level back at the last
 * checkpoint it reached, forgets the bosses announced that have not entered,
 * and removes every body but the ships of the players, each with an
 * EntityDestruction Removed.
 *
 * It works on one world: the game builds a new one with each new world. Not
 * thread-safe: use it from the simulation thread only.
 */
class LevelDirector {
 public:
  /** @throws UnknownLevelException as stagesOfLevel() does. */
  explicit LevelDirector(LevelDirectorParts parts);

  /** @brief Lets one tick of the level pass. */
  void onTick();

  /** @returns Whether the last stage returned and no enemy is left. */
  [[nodiscard]] bool cleared() const;

  /** @returns Whether a stage of the level failed. */
  [[nodiscard]] bool failed() const;

  /**
   * @brief Puts the level back at its last checkpoint, without any body but
   * the ships of the players.
   * @returns Whether the level had reached a checkpoint and has not failed;
   * nothing changes otherwise.
   */
  bool restartFromCheckpoint();

  /** @returns Where the level stands: its scroll, its biome and its last
   * checkpoint, for whatever in the game follows the scroll. */
  [[nodiscard]] const game::LevelProgress& progress() const;

 private:
  void announceChange();
  void removeEveryBodyButTheShips() const;

  LevelDirectorParts parts_;
  game::LevelProgress progress_;
  LevelBosses bosses_;
  game::LuauLevelRun run_;
  logging::Logger logger_;
  /** The biome of the last LevelChange published; nothing before the first
   * and after a restart, so the next tick publishes one. */
  std::optional<std::string> announcedBiome_;
  bool announcedMoving_{false};
};

}  // namespace rtype::server
