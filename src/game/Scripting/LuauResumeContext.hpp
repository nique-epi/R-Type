#pragma once

#include <cstdint>
#include <optional>
#include "IEntityOrders.hpp"

struct lua_State;

namespace rtype::game {

struct LuauBehaviourState;

/** @brief What the functions the scripts call know of the resume in progress:
 * the thread of the body that steps and its behaviour state, both nullptr
 * while none steps, the ticks it asked to wait, and the game that carries out
 * the bodies they order, nullptr outside a game. */
struct LuauResumeContext {
  lua_State* resumedThread{nullptr};
  LuauBehaviourState* resumedBehaviourState{nullptr};
  std::optional<std::uint32_t> waitTicks = std::nullopt;
  IEntityOrders* orders{nullptr};
};

}  // namespace rtype::game
