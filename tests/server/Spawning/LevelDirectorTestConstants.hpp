#pragma once

#include <cstddef>
#include <string_view>
#include "Position.hpp"

namespace rtype::server::testing {

/** @brief The name the level of these tests is registered under, and one no
 * script registers. */
constexpr std::string_view DIRECTED_LEVEL = "directed";
constexpr std::string_view UNREGISTERED_LEVEL = "nowhere";

/** @brief A level of one stage that names its biome and ends. */
constexpr std::string_view LEVEL_OF_ONE_BIOME = R"(
level.register("directed", { function()
  level.biome("wreck")
end })
)";
constexpr std::string_view WRECK_BIOME = "wreck";

/** @brief What a level that scrolls tells: its speed, in logical units per
 * second, and the 2 units it scrolls in one tick. */
constexpr float SPEED_OF_A_SCROLLING_LEVEL = 120.0F;
constexpr float SPEED_OF_A_STOPPED_LEVEL = 0.0F;
constexpr float SCROLL_OF_ONE_TICK = 2.0F;
constexpr float SCROLL_TOLERANCE = 0.001F;

/** @brief One second of ticks, and how far a level scrolls in it. */
constexpr std::size_t TICKS_OF_ONE_SECOND = 60;
constexpr float SCROLL_OF_ONE_SECOND = 120.0F;

/** @brief A level whose stage names a biome, waits 50 ms (3 ticks), then
 * pauses the scroll and waits again: the pause is made at tick 4. */
constexpr std::string_view LEVEL_THAT_PAUSES = R"(
level.register("directed", { function()
  level.biome("wreck")
  level.wait(50)
  level.pause_scroll()
  level.wait(50)
end })
)";

/** @brief A level whose stage names a biome, waits 50 ms (3 ticks), then
 * names another and waits again: the second is named at tick 4. */
constexpr std::string_view LEVEL_OF_TWO_BIOMES = R"(
level.register("directed", { function()
  level.biome("wreck")
  level.wait(50)
  level.biome("caves")
  level.wait(50)
end })
)";
constexpr std::string_view CAVES_BIOME = "caves";
constexpr std::size_t TICK_OF_THE_SECOND_ACT = 4;

/** @brief A level whose stage waits for a long time. */
constexpr std::string_view LEVEL_THAT_WAITS = R"(
level.register("directed", { function()
  level.wait(60000)
end })
)";

/** @brief A level whose stage fails. */
constexpr std::string_view LEVEL_THAT_FAILS = R"(
level.register("directed", { function()
  error("broken")
end })
)";

/** @brief A level of two stages: the first waits 50 ms, so it ends at tick 4
 * after 3 ticks of scroll, 6 logical units, where the checkpoint is; the
 * second names a biome and waits for a long time. */
constexpr std::string_view LEVEL_OF_TWO_STAGES = R"(
level.register("directed", {
  function()
    level.wait(50)
  end,
  function()
    level.biome("caves")
    level.wait(60000)
  end,
})
)";
constexpr std::size_t TICKS_UNTIL_THE_CHECKPOINT = 4;
constexpr std::size_t TICKS_INTO_THE_SECOND_STAGE = 30;
constexpr float SCROLL_ONE_TICK_AFTER_THE_CHECKPOINT = 8.0F;

/** @brief The shipped sheets of an enemy and of the ship of a player, and
 * where the tests put them. */
constexpr std::string_view LEVEL_ENEMY_SHEET = "drifter";
constexpr std::string_view LEVEL_SHIP_SHEET = "ship";
constexpr game::Position WHERE_THE_ENEMY_IS{.x = 900.0F, .y = 300.0F};
constexpr game::Position WHERE_THE_SHIP_IS{.x = 200.0F, .y = 500.0F};

/** @brief Where a body that follows the scroll is after one second of a level
 * scrolling at 120 units a second: 120 units left of WHERE_THE_ENEMY_IS; and
 * after the first five ticks of LEVEL_THAT_PAUSES, which scrolls three ticks
 * of 2 units before its pause: 6 units left of it. */
constexpr float SCROLLED_ONE_SECOND_X = 780.0F;
constexpr float SCROLLED_UNTIL_THE_PAUSE_X = 894.0F;
constexpr std::size_t TICKS_PAST_THE_PAUSE = 5;

}  // namespace rtype::server::testing
