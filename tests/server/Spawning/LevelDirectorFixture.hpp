#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string_view>
#include <vector>
#include "BehaviourScripts.hpp"
#include "EventBus.hpp"
#include "LdtkPlacement.hpp"
#include "LevelChange.hpp"
#include "LevelDirector.hpp"
#include "Position.hpp"
#include "RecordingEntityOrders.hpp"
#include "World.hpp"

namespace rtype::server::testing {

/**
 * @brief A world of the shipped sheets and the LevelDirector of the level a
 * test script registers, ticked as a game ticks it; the changes of the level
 * and the removals it publishes are recorded once the bus delivers them, and
 * the enemies it orders are recorded as asked.
 */
class LevelDirectorFixture : public ::testing::Test {
 protected:
  LevelDirectorFixture();

  /** @brief Gives the level directed next the enemies @p placements; call it
   * before direct(). */
  void placeEnemies(std::vector<game::LdtkPlacement> placements);
  /** @brief Places the one Drifter of these tests, at
   * WHERE_THE_DRIFTER_IS_PLACED; call it before direct(). */
  void placeTheDrifter();
  /** @brief Runs @p script and directs the level DIRECTED_LEVEL it
   * registers, with the enemies placeEnemies() gave it. */
  void direct(std::string_view script);
  /** @brief Lets @p count ticks pass, the bus delivering after each. */
  void tick(std::size_t count);
  /** @returns The network id of a body of @p sheet put at @p position. */
  std::uint32_t place(std::string_view sheet, game::Position position);
  /** @returns The director of the level. */
  [[nodiscard]] LevelDirector& director();
  /** @returns The scripts of the test. */
  [[nodiscard]] game::BehaviourScripts& scripts();
  /** @returns The changes of the level published so far. */
  [[nodiscard]] const std::vector<game::LevelChange>& changes() const;
  /** @returns The network ids of the bodies removed so far. */
  [[nodiscard]] const std::vector<std::uint32_t>& removed() const;
  /** @returns The bodies the director asked for so far, in order. */
  [[nodiscard]] const std::vector<RecordedSpawn>& spawned() const;

 private:
  game::World world_;
  engine::EventBus events_;
  game::BehaviourScripts scripts_;
  RecordingEntityOrders orders_;
  std::vector<game::LdtkPlacement> placements_;
  std::unique_ptr<LevelDirector> director_;
  std::vector<game::LevelChange> changes_;
  std::vector<std::uint32_t> removed_;
};

}  // namespace rtype::server::testing
