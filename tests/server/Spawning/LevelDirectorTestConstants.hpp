#pragma once

#include <cstddef>
#include <cstdint>
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

/** @brief The network id the recorded orders give every body. */
constexpr std::uint32_t PLACED_ENEMY_ID = 77;

/** @brief A placed Drifter, 96 units wide, whose center is at x 2224: its left
 * edge reaches the right edge of the playfield once the level scrolled 256
 * units. 128 ticks of scroll fall just short of it (a tick is 16666666 ns,
 * so 128 ticks are 255.99999 units), 129 ticks are 258 units: it comes in at
 * the step of tick 130, where it appears at x 1966. */
constexpr game::Position WHERE_THE_DRIFTER_IS_PLACED{.x = 2224.0F, .y = 540.0F};
constexpr std::size_t TICKS_BEFORE_THE_DRIFTER_COMES_IN = 129;
constexpr float WHERE_THE_DRIFTER_APPEARS_X = 1966.0F;

/** @brief A level whose stage asks for the placed enemies, then waits for a
 * long time. */
constexpr std::string_view LEVEL_THAT_SPAWNS_THE_PLACED = R"(
level.register("directed", { function()
  level.spawn_placed()
  level.wait(60000)
end })
)";

/** @brief Two placed Drifters whose left edge comes in once the level
 * scrolled 4 units (at the step of tick 3) and 20 units (at the step of tick
 * 12, since 10 ticks of scroll fall just short of 20): at the checkpoint of
 * LEVEL_OF_TWO_STAGES_THAT_SPAWNS_THE_PLACED, 6 units, the first has come in
 * and the second has not. */
constexpr game::Position WHERE_THE_EARLY_DRIFTER_IS{.x = 1972.0F, .y = 300.0F};
constexpr game::Position WHERE_THE_LATE_DRIFTER_IS{.x = 1988.0F, .y = 780.0F};
constexpr std::size_t TICKS_UNTIL_BOTH_CAME_IN = 12;
constexpr std::size_t TICKS_UNTIL_THE_LATE_ONE_COMES_IN_AGAIN = 11;
constexpr std::size_t SPAWNS_BEFORE_THE_RESTART = 2;
constexpr std::size_t SPAWNS_AFTER_THE_RESTART = 3;

/** @brief A level of two stages: the first asks for the placed enemies and
 * waits 50 ms, so it ends at tick 4 after 3 ticks of scroll, 6 units, where
 * the checkpoint is; the second waits for a long time. */
constexpr std::string_view LEVEL_OF_TWO_STAGES_THAT_SPAWNS_THE_PLACED = R"(
level.register("directed", {
  function()
    level.spawn_placed()
    level.wait(50)
  end,
  function()
    level.wait(60000)
  end,
})
)";

}  // namespace rtype::server::testing
