#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <memory>
#include <vector>
#include "BossArrival.hpp"
#include "EntitySheets.hpp"
#include "EventBus.hpp"
#include "LevelBosses.hpp"
#include "PlayerRoster.hpp"
#include "World.hpp"

namespace rtype::server::testing {

/** @brief The bosses of a level, among the warlord and the minion of the
 * stage tests, in a world nothing moves; the arrivals they announce are
 * recorded. */
class LevelBossesFixture : public ::testing::Test {
 protected:
  LevelBossesFixture();

  /** @brief Builds the bosses again for a difficulty that gives bosses
   * @p percent percent of the health of their sheet. */
  void playDifficulty(int percent);

  /** @returns The bosses of the level. */
  [[nodiscard]] LevelBosses& bosses();

  /** @brief Lets @p count ticks of the bosses pass. */
  void tick(std::size_t count);

  /** @returns The BossArrival announced so far, once delivered. */
  [[nodiscard]] std::vector<game::BossArrival> arrivals();

  /** @returns The full health of each boss of the world, in the order they
   * entered (their network ids). */
  [[nodiscard]] std::vector<int> bossHealths();

 private:
  game::EntitySheets sheets_;
  game::World world_;
  PlayerRoster roster_;
  engine::EventBus events_;
  std::vector<game::BossArrival> arrivals_;
  std::unique_ptr<LevelBosses> bosses_;
};

}  // namespace rtype::server::testing
