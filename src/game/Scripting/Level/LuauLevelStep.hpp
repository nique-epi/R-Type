#pragma once

#include <optional>
#include "LuauLevelWait.hpp"

struct lua_State;

namespace rtype::engine {

class ComponentRegistry;

}  // namespace rtype::engine

namespace rtype::game {

struct LevelProgress;

/** @brief What the functions of `level` know of the game and of the step in
 * progress: the progress of the level the game plays and the components of
 * its world, nullptr outside such a game, the thread of the stage that steps,
 * nullptr when none does, and the wait that stage asked for. */
struct LuauLevelStep {
  LevelProgress* progress{nullptr};
  engine::ComponentRegistry* components{nullptr};
  lua_State* resumedThread{nullptr};
  std::optional<LuauLevelWait> wait = std::nullopt;
};

}  // namespace rtype::game
