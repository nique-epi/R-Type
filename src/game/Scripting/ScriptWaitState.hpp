#pragma once

#include <cstdint>
#include <optional>

struct lua_State;

namespace rtype::game {

/** @brief What `behaviours.wait` tells the step in progress: the thread of the
 * body that steps, and the ticks it asked to wait. */
struct ScriptWaitState {
  lua_State* stepping{nullptr};
  std::optional<std::uint32_t> waitTicks = std::nullopt;
};

}  // namespace rtype::game
