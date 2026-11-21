#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include "LevelCheckpoint.hpp"
#include "LevelDirectorParts.hpp"
#include "LevelProgress.hpp"
#include "Logger.hpp"
#include "LuauLevelEntry.hpp"
#include "LuauLevelRun.hpp"
#include "LuauLevels.hpp"
#include "PlacedEnemySpawner.hpp"
#include "PlayBand.hpp"

namespace rtype::server {

/**
 * @returns What the scripts registered as the level @p level among
 * @p levels.
 * @throws UnknownLevelException, naming the levels the scripts registered,
 * when none of them is @p level.
 */
[[nodiscard]] const game::LuauLevelEntry& entryOfLevel(
    const game::LuauLevels& levels, const std::string& level);

/**
 * @returns Where a game that starts at @p stage, from FIRST_LEVEL_STAGE, of
 * @p entry, the level @p level, starts: nothing at FIRST_LEVEL_STAGE, the
 * start the script declares for that stage otherwise.
 * @throws UnknownLevelStartException when it declares none.
 */
[[nodiscard]] std::optional<game::LevelCheckpoint> startOfStage(
    const game::LuauLevelEntry& entry, const std::string& level,
    std::size_t stage);

/**
 * @brief Directs the level a game plays in one world: it steps the stages
 * its script registered (see game::LuauLevelRun), scrolls the level, and
 * tells the game what the players must see of it.
 *
 * Each onTick() steps the stage due, makes the placed enemies due appear once
 * a stage asked for them with `level.spawn_placed()` (see
 * game::PlacedEnemySpawner), then scrolls the level by one tick unless its
 * script paused the scroll, carries every body that follows the scroll as far
 * as the level scrolled (game::carryAlongTheScroll()), then counts down the
 * transition a stage plays with `level.transition(ms)`, if any, and lets the
 * scroll move on once it ended, before the world moves its ships and tests its
 * collisions. A game::LevelChange is published on the bus of the game at the
 * first tick, then each time the biome changes, the scroll stops or starts
 * again, a transition starts or ends, or the level restarts from a checkpoint,
 * which forgets the transition it played.
 *
 * The level is cleared once its last stage returned and no enemy is left in
 * the world. A stage that fails makes the level failed, for good, which the
 * journal says once. restartFromCheckpoint() puts the level back at the last
 * checkpoint it reached and removes every body but the ships of the players,
 * each with an EntityDestruction Removed; the enemies placed after that
 * checkpoint then come in again.
 *
 * A director built for a stage after the first starts the level there, as
 * restartFromCheckpoint() would from the start the script declares for it,
 * which then stays the checkpoint of the level.
 *
 * It works on one world: the game builds a new one with each new world. Not
 * thread-safe: use it from the simulation thread only.
 */
class LevelDirector {
 public:
  /** @throws UnknownLevelException as entryOfLevel() does, and
   * UnknownLevelStartException as startOfStage() does. */
  explicit LevelDirector(LevelDirectorParts parts);

  /** @brief Lets one tick of the level pass. */
  void onTick();

  /** @returns Whether the last stage returned and no enemy is left. */
  [[nodiscard]] bool cleared() const;

  /** @returns Whether a stage of the level failed. */
  [[nodiscard]] bool failed() const;

  /**
   * @brief Puts the level back at its last checkpoint, without any body but
   * the ships of the players. Its biome is told begun OPENING_BIOME_LEAD
   * before the checkpoint at the latest, as a level that opens in a biome:
   * nothing is left behind the entrance of a background after a restart, so
   * the players start inside it.
   * Each restart spends one of the continues of the team
   * (LevelProgress::continuesLeft), and the change of the level that follows
   * tells how many are left.
   * @returns Whether the level had reached a checkpoint, has not failed and
   * its team held a continue; nothing changes otherwise.
   */
  bool restartFromCheckpoint();

  /** @returns Where the level stands: its scroll, its biome and its last
   * checkpoint, for whatever in the game follows the scroll. */
  [[nodiscard]] const game::LevelProgress& progress() const;

  /** @returns The rows the ships fly between in the biome the level shows:
   * the band the level declares for it, the whole play area otherwise. */
  [[nodiscard]] game::PlayBand band() const;

 private:
  void spawnDuePlacements();
  void advanceTransition();
  void announceChange();
  /** @brief Puts the level back at its last checkpoint, as
   * restartFromCheckpoint() says, without spending a continue: a game started
   * at a stage gets there this way.
   * @returns Whether the level had reached a checkpoint and has not failed. */
  bool returnToCheckpoint();

  LevelDirectorParts parts_;
  /** What the scripts registered as the level, found once. */
  std::reference_wrapper<const game::LuauLevelEntry> entry_;
  game::LevelProgress progress_;
  game::LuauLevelRun run_;
  game::PlacedEnemySpawner placer_;
  logging::Logger logger_;
  /** The biome of the last LevelChange published; nothing before the first
   * and after a restart, so the next tick publishes one. */
  std::optional<std::string> announcedBiome_;
  bool announcedMoving_{false};
  bool announcedTransitioning_{false};
};

}  // namespace rtype::server
