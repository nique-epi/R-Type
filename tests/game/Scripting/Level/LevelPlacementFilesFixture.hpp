#pragma once

#include <gtest/gtest.h>
#include <filesystem>
#include <span>
#include <string_view>
#include "BehaviourScripts.hpp"
#include "BehaviourScriptsFixtures.hpp"
#include "LuauLevels.hpp"
#include "TemporaryFolder.hpp"

/** @brief Content folders a test writes below a temporary folder, and the
 * scripts that register their levels, read as a server reads them. */
class LevelPlacementFilesFixture : public ::testing::Test {
 protected:
  LevelPlacementFilesFixture();

  /** @brief Writes the script that registers PLACED_LEVEL and the LDtk file
   * of @p entities, a Mobs layer of one level, in the content folder
   * @p content. */
  void writeContent(std::string_view content, std::string_view entities);
  /** @returns The content folder @p content of the test. */
  [[nodiscard]] std::filesystem::path contentFolder(
      std::string_view content) const;
  /** @brief Runs the scripts of @p folders, then reads the placements of their
   * levels with the sheets the game ships. */
  void readPlacements(std::span<const std::filesystem::path> folders);
  /** @returns The levels the scripts registered. */
  [[nodiscard]] rtype::game::LuauLevels& levels();

 private:
  TemporaryFolder root_;
  FailureLog log_;
  rtype::game::BehaviourScripts scripts_;
};
