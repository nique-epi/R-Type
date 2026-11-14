#pragma once

#include <functional>
#include <vector>
#include "ComponentRegistry.hpp"
#include "ILevelBosses.hpp"
#include "LevelProgress.hpp"
#include "LuauLevels.hpp"
#include "ScriptMachine.hpp"

namespace rtype::game {

/** @brief What a LuauLevelRun is built from; every reference must outlive
 * it. */
struct LuauLevelRunParts {
  /** @brief The machine the stages run on. */
  std::reference_wrapper<engine::ScriptMachine> machine;
  /** @brief What exposes `level` on that machine, told which stage steps. */
  std::reference_wrapper<LuauLevels> levels;
  /** @brief The stages of the level, as LuauLevels::stagesOf() gives them;
   * one at least. */
  std::vector<int> stages;
  /** @brief The progress of the level, which the stages read and direct. */
  std::reference_wrapper<LevelProgress> progress;
  /** @brief The components of the world the level is played in. */
  std::reference_wrapper<engine::ComponentRegistry> components;
  /** @brief The bosses the stages bring in. */
  std::reference_wrapper<ILevelBosses> bosses;
};

}  // namespace rtype::game
