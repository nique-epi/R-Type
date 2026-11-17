#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "BossStage.hpp"
#include "BossStageTestConstants.hpp"
#include "EntitySheets.hpp"
#include "RecordingEntityOrders.hpp"
#include "RefusingEntityOrders.hpp"
#include "World.hpp"

namespace rtype::server::testing {

/**
 * @brief A world holding the warlord and the minion of the stage tests, and
 * the boss stage of a level whose boss is the warlord; the bodies it orders
 * are recorded, and never appear by themselves. Nothing moves the bodies.
 */
class BossStageFixture : public ::testing::Test {
 protected:
  BossStageFixture();

  /** @brief Builds the stage again, its level ending with @p boss. */
  void buildStage(std::optional<std::string> boss);

  /** @brief Builds the stage again, its level ending with the warlord, with
   * orders that refuse every body. */
  void refuseTheBoss();

  /** @brief Runs @p count ticks of the stage, the last wave being over or
   * not as @p lastWaveFinished says. */
  void runTicks(std::size_t count, bool lastWaveFinished);

  /** @returns The bodies the stage ordered so far. */
  [[nodiscard]] const std::vector<RecordedSpawn>& orders() const;

  /** @brief Makes the warlord appear, as the game does at the end of the
   * tick after the stage ordered it. */
  void addBoss();

  /** @brief Makes a minion appear. */
  void addMinion();

  /** @brief Destroys the bosses of the world, as if they were killed. */
  void destroyTheBosses();

  /** @returns Whether the stage says the level is cleared. */
  [[nodiscard]] bool cleared() const;

 private:
  game::EntitySheets sheets_;
  game::World world_;
  RecordingEntityOrders orders_{ORDERED_BOSS_ID};
  RefusingEntityOrders refusingOrders_;
  std::unique_ptr<BossStage> stage_;
};

}  // namespace rtype::server::testing
