#pragma once

#include <functional>
#include "ComponentRegistry.hpp"
#include "LevelProgress.hpp"
#include "LuauLevelEntry.hpp"
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
  /** @brief What the scripts registered as the level, as
   * LuauLevels::entryOf() gives it: its stages, one at least, and the
   * enemies it places. */
  std::reference_wrapper<const LuauLevelEntry> entry;
  /** @brief The progress of the level, which the stages read and direct. */
  std::reference_wrapper<LevelProgress> progress;
  /** @brief The components of the world the level is played in. */
  std::reference_wrapper<engine::ComponentRegistry> components;
};

}  // namespace rtype::game
