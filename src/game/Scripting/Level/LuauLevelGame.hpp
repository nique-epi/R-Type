#pragma once

#include <functional>
#include "ComponentRegistry.hpp"
#include "ILevelBosses.hpp"
#include "LevelProgress.hpp"
#include "LuauLevelEntry.hpp"

namespace rtype::game {

/** @brief The game whose level the functions of `level` direct: the progress
 * of its level, the components of its world, the bosses it brings in, and
 * what the scripts registered as that level. */
struct LuauLevelGame {
  std::reference_wrapper<LevelProgress> progress;
  std::reference_wrapper<engine::ComponentRegistry> components;
  std::reference_wrapper<ILevelBosses> bosses;
  std::reference_wrapper<const LuauLevelEntry> entry;
};

}  // namespace rtype::game
