#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include "Position.hpp"

/** @brief `pacer` waits 50 ms going left, then goes down for one tick, then
 * starts again. */
constexpr std::string_view PACER_SCRIPT = R"(
behaviours.register("pacer", {}, function(view)
  behaviours.wait(50, { direction = { x = -1, y = 0 } })
  return { direction = { x = 0, y = 1 } }
end)
)";
constexpr std::string_view PACER_BEHAVIOUR = "pacer";

/** @brief 50 ms at 60 ticks a second: the tick of the call and two more. */
constexpr std::size_t TICKS_OF_THE_PACER_WAIT = 3;

/** @brief `hoarder` keeps a table of 4096 numbers and waits for a second at
 * a time, for ever. */
constexpr std::string_view HOARDER_SCRIPT = R"(
behaviours.register("hoarder", {}, function(view)
  local hoard = table.create(4096, 1.5)
  while true do
    behaviours.wait(1000, { direction = { x = -hoard[1], y = 0 } })
  end
end)
)";
constexpr std::string_view HOARDER_BEHAVIOUR = "hoarder";
constexpr std::size_t HOARDING_BODIES = 20;

/** @brief What the hoards of HOARDING_BODIES bodies hold at least: 4096
 * values of 16 bytes each. */
constexpr std::size_t BYTES_OF_THE_HOARDS = HOARDING_BODIES * 4096 * 16;

/** @brief `picky` fails left of x 500, and goes left elsewhere. */
constexpr std::string_view PICKY_SCRIPT = R"(
behaviours.register("picky", {}, function(view)
  if view.position.x < 500 then
    error("too far left")
  end
  return { direction = { x = -1, y = 0 } }
end)
)";
constexpr std::string_view PICKY_BEHAVIOUR = "picky";
constexpr std::uint32_t FAILING_BODY_ID = 7;

/** @brief `runaway` never ends a step left of x 500, and goes left
 * elsewhere. */
constexpr std::string_view RUNAWAY_SCRIPT = R"(
behaviours.register("runaway", {}, function(view)
  while view.position.x < 500 do
  end
  return { direction = { x = -1, y = 0 } }
end)
)";
constexpr std::string_view RUNAWAY_BEHAVIOUR = "runaway";

/** @brief Two bodies, one on each side of x 500. */
constexpr rtype::game::Position BODY_ON_THE_LEFT{.x = 100.0F, .y = 500.0F};
constexpr rtype::game::Position BODY_ON_THE_RIGHT{.x = 1000.0F, .y = 500.0F};

/** @brief `builder` creates a drifter at (10, 20), then removes it. */
constexpr std::string_view BUILDER_SCRIPT = R"(
behaviours.register("builder", {}, function(view)
  local id = behaviours.spawn("drifter", { x = 10, y = 20 })
  behaviours.destroy(id)
  return {}
end)
)";
constexpr std::string_view BUILDER_BEHAVIOUR = "builder";
constexpr std::string_view BUILT_TYPE = "drifter";
constexpr rtype::game::Position BUILT_AT{.x = 10.0F, .y = 20.0F};
constexpr std::uint32_t GIVEN_NETWORK_ID = 42;

/** @brief `yielder` yields without behaviours.wait. */
constexpr std::string_view YIELDER_SCRIPT = R"(
behaviours.register("yielder", {}, function(view)
  coroutine.yield({ direction = { x = -1, y = 0 } })
  return {}
end)
)";
constexpr std::string_view YIELDER_BEHAVIOUR = "yielder";
