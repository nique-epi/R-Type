#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "BehaviourScripts.hpp"
#include "BehaviourScriptsFixtures.hpp"
#include "BiomeGround.hpp"
#include "ComponentRegistry.hpp"
#include "EntityRegistry.hpp"
#include "LdtkPlacement.hpp"
#include "LevelProgress.hpp"
#include "LuauLevelRun.hpp"

/** @brief The level a test plays tick after tick as a game does: each tick
 * gives the scripts their budget, steps the level, then scrolls it. */
class LuauLevelRunFixture : public ::testing::Test {
 protected:
  LuauLevelRunFixture();

  /** @brief Gives the level played next the enemies @p placements; call it
   * before play(). */
  void placeEnemies(std::vector<rtype::game::LdtkPlacement> placements);
  /** @brief Gives the level played next @p ground in the biome @p biome;
   * call it before play(). */
  void giveGround(std::string biome, rtype::game::BiomeGround ground);
  /** @brief Runs @p script and starts the run of the level TEST_LEVEL it
   * registers, with the enemies placeEnemies() gave it. */
  void play(std::string_view script);
  /** @brief Lets @p count ticks pass. */
  void tick(std::size_t count);
  /** @brief Puts one enemy in the world of the level. */
  void addEnemy();
  /** @returns The run of the level played. */
  [[nodiscard]] rtype::game::LuauLevelRun& run();
  /** @returns The progress of the level played. */
  [[nodiscard]] rtype::game::LevelProgress& progress();

 private:
  FailureLog log_;
  rtype::game::BehaviourScripts scripts_;
  rtype::engine::EntityRegistry entities_;
  rtype::engine::ComponentRegistry components_;
  rtype::game::LevelProgress progress_;
  std::vector<rtype::game::LdtkPlacement> placements_;
  std::map<std::string, rtype::game::BiomeGround, std::less<>> grounds_;
  std::unique_ptr<rtype::game::LuauLevelRun> run_;
};
