#pragma once

#include <cstdint>
#include <optional>
#include "IEntityOrders.hpp"
#include "LuauPreHit.hpp"
#include "LuauSteppedBody.hpp"

struct lua_State;

namespace rtype::game {

/** @brief What the functions the scripts call know of the resume in progress:
 * the thread of the body that steps, the ticks it asked to wait, the game
 * that carries out the bodies they order, nullptr outside a game, the body
 * whose components its handle reaches, and where that body keeps the function
 * `behaviours.pre_hit` declares, nullptr between two steps. */
struct LuauResumeContext {
  lua_State* resumedThread{nullptr};
  std::optional<std::uint32_t> waitTicks = std::nullopt;
  IEntityOrders* orders{nullptr};
  LuauSteppedBody steppedBody{};
  std::optional<LuauPreHit>* preHit{nullptr};
};

}  // namespace rtype::game
