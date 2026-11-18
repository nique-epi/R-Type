#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::server::testing {

/** @brief The levels of the game flow tests. `two_stages` waits 50 ms
 * (3 ticks) in its first stage, which ends at the fourth tick of the game,
 * where its checkpoint is, then waits for a long time in its second.
 * `one_stage` ends at once, `broken` fails at once, `sends_a_bydo` creates
 * a Drifter at its first tick, then waits for a long time, and
 * `fights_a_bydo` creates a Drifter, waits 50 ms, then ends once no enemy is
 * left, as a stage that brings a boss in does. */
constexpr std::string_view LEVELS_OF_THE_FLOW_TESTS = R"(
level.register("two_stages", {
  function()
    level.wait(50)
  end,
  function()
    level.wait(60000)
  end,
})
level.register("one_stage", { function() end })
level.register("broken", { function() error("broken") end })
level.register("sends_a_bydo", { function()
  behaviours.spawn("drifter", { x = 1984, y = 540 })
  level.wait(60000)
end })
level.register("fights_a_bydo", { function()
  behaviours.spawn("drifter", { x = 1984, y = 540 })
  level.wait(50)
  while level.enemies() > 0 do
    level.wait(50)
  end
end })
)";
constexpr std::string_view LEVEL_OF_TWO_STAGES_NAME = "two_stages";
constexpr std::string_view LEVEL_OF_ONE_STAGE_NAME = "one_stage";
constexpr std::string_view BROKEN_LEVEL_NAME = "broken";
constexpr std::string_view LEVEL_THAT_SENDS_A_BYDO_NAME = "sends_a_bydo";
constexpr std::string_view LEVEL_THAT_FIGHTS_A_BYDO_NAME = "fights_a_bydo";

/** @brief A third of a second of ticks: the Drifter of `fights_a_bydo`,
 * created past the right edge, is still in the world. */
constexpr std::size_t TICKS_WHILE_THE_BYDO_LIVES = 20;

/** @brief The name the scripts of these tests are read under. */
constexpr std::string_view FLOW_LEVELS_SCRIPT_NAME = "flow_levels.luau";

/** @brief Ticks of a game in progress before, and past, the checkpoint of
 * `two_stages`, reached at its fourth tick. */
constexpr std::size_t TICKS_BEFORE_THE_CHECKPOINT = 2;
constexpr std::size_t TICKS_PAST_THE_CHECKPOINT = 6;

/** @brief The lives the rules of a game give each player by default. */
constexpr std::uint8_t LIVES_OF_THE_RULES = 3;

}  // namespace rtype::server::testing
