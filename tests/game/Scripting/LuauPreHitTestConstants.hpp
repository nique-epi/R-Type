#pragma once

#include <cstddef>
#include <string_view>
#include "IncomingHit.hpp"
#include "Position.hpp"
#include "ScriptingConstants.hpp"

/** @brief `shielded` cancels every hit, and waits for ever. */
constexpr std::string_view SHIELDED_SCRIPT = R"(
behaviours.register("shielded", {}, function(e)
  behaviours.pre_hit(function(hit) return false end)
  while true do
    behaviours.wait(1000, {})
  end
end)
)";
constexpr std::string_view SHIELDED_BEHAVIOUR = "shielded";

/** @brief `open` declares a pre_hit function that returns nothing. */
constexpr std::string_view OPEN_SCRIPT = R"(
behaviours.register("open", {}, function(e)
  behaviours.pre_hit(function(hit) end)
  return {}
end)
)";
constexpr std::string_view OPEN_BEHAVIOUR = "open";

/** @brief `bare` declares no pre_hit function. */
constexpr std::string_view BARE_SCRIPT = R"(
behaviours.register("bare", {}, function(e)
  return {}
end)
)";
constexpr std::string_view BARE_BEHAVIOUR = "bare";

/** @brief `weak_spot` cancels only the hit of missile 42 lying 20 units right
 * of its center and 20 above it. */
constexpr std::string_view WEAK_SPOT_SCRIPT = R"(
behaviours.register("weak_spot", {}, function(e)
  behaviours.pre_hit(function(hit)
    return not (hit.missile == 42 and hit.offset.x == 20 and hit.offset.y == -20)
  end)
  return {}
end)
)";
constexpr std::string_view WEAK_SPOT_BEHAVIOUR = "weak_spot";
constexpr rtype::game::IncomingHit HIT_ON_THE_WEAK_SPOT{
    .missileNetworkId = 42,
    .offset = rtype::game::Position{.x = 20.0F, .y = -20.0F}};

/** @brief `cracked` declares a pre_hit function that fails, and goes left. */
constexpr std::string_view CRACKED_SCRIPT = R"(
behaviours.register("cracked", {}, function(e)
  behaviours.pre_hit(function(hit) error("cracked") end)
  return { direction = { x = -1, y = 0 } }
end)
)";
constexpr std::string_view CRACKED_BEHAVIOUR = "cracked";

/** @brief `stalling` declares a pre_hit function that yields `false`, as a
 * return would cancel the hit. */
constexpr std::string_view STALLING_SCRIPT = R"(
behaviours.register("stalling", {}, function(e)
  behaviours.pre_hit(function(hit) coroutine.yield(false) end)
  return {}
end)
)";
constexpr std::string_view STALLING_BEHAVIOUR = "stalling";

/** @brief `dropping` cancels every hit on its first tick, waits one tick,
 * then forgets its pre_hit function with `nil`. */
constexpr std::string_view DROPPING_SCRIPT = R"(
behaviours.register("dropping", {}, function(e)
  behaviours.pre_hit(function(hit) return false end)
  behaviours.wait(0, {})
  behaviours.pre_hit(nil)
  while true do
    behaviours.wait(1000, {})
  end
end)
)";
constexpr std::string_view DROPPING_BEHAVIOUR = "dropping";

/** @brief `brittle` declares a pre_hit function that cancels every hit, then
 * fails in the same step. */
constexpr std::string_view BRITTLE_SCRIPT = R"(
behaviours.register("brittle", {}, function(e)
  behaviours.pre_hit(function(hit) return false end)
  error("brittle")
end)
)";
constexpr std::string_view BRITTLE_BEHAVIOUR = "brittle";

/** @brief A script that declares a pre_hit function while it is read, outside
 * the function of any body. */
constexpr std::string_view PRE_HIT_AT_READING_SCRIPT = R"(
behaviours.pre_hit(function(hit) return false end)
)";

/** @brief Any hit, for a script that judges every hit alike. */
constexpr rtype::game::IncomingHit ANY_HIT{};

/** @brief How many steps stopped by their step budget spend the budget of a
 * tick: 8 ms over 2 ms, 4; each runs a little past its 2 ms before it is
 * stopped, so 4 spend it whole. */
constexpr auto STEPS_THAT_SPEND_A_TICK = static_cast<std::size_t>(
    rtype::engine::SCRIPT_TICK_BUDGET / rtype::engine::SCRIPT_STEP_BUDGET);

/** @brief `guarded` cancels every hit and waits for ever, as `shielded`, and
 * `stuck` never ends a step. */
constexpr std::string_view GUARDED_AND_STUCK_SCRIPT = R"(
behaviours.register("guarded", {}, function(e)
  behaviours.pre_hit(function(hit) return false end)
  while true do
    behaviours.wait(1000, {})
  end
end)
behaviours.register("stuck", {}, function(e)
  while true do
  end
end)
)";
constexpr std::string_view GUARDED_BEHAVIOUR = "guarded";
constexpr std::string_view STUCK_BEHAVIOUR = "stuck";
