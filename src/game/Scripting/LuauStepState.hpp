#pragma once

#include <cstdint>
#include <optional>
#include "IBodyOrders.hpp"

struct lua_State;

namespace rtype::game {

/** @brief What the functions the scripts call know of the step in progress:
 * the thread of the body that steps, the ticks it asked to wait, and the
 * game that carries out the bodies they order, nullptr outside a game. */
struct LuauStepState {
  lua_State* stepping{nullptr};
  std::optional<std::uint32_t> waitTicks = std::nullopt;
  IBodyOrders* orders{nullptr};
};

}  // namespace rtype::game
