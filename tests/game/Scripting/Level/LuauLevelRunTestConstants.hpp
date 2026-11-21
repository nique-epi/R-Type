#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include "Position.hpp"
#include "TimeConstants.hpp"

/** @brief The name every level of these tests is registered under. */
constexpr std::string_view TEST_LEVEL = "test_level";

/** @brief A wait of 50 ms, the 3 ticks every waiting script of these tests
 * waits: a stage that starts at tick 1 and waits it goes on at tick 4. */
constexpr std::size_t TICKS_UNTIL_THE_LAST_OF_THE_WAIT = 3;
constexpr std::size_t TICKS_UNTIL_THE_WAIT_IS_OVER = 4;

/** @brief A level of one stage that names a biome and ends. */
constexpr std::string_view NAMES_A_BIOME = R"(
level.register("test_level", { function()
  level.biome("station")
end })
)";
constexpr std::string_view STATION_BIOME = "station";

/** @brief A level whose stage names a biome, waits 50 ms and names
 * another. */
constexpr std::string_view WAITS_BETWEEN_TWO_BIOMES = R"(
level.register("test_level", { function()
  level.biome("before")
  level.wait(50)
  level.biome("after")
end })
)";
constexpr std::string_view BIOME_BEFORE_THE_WAIT = "before";
constexpr std::string_view BIOME_AFTER_THE_WAIT = "after";

/** @brief A level whose stage waits until it scrolled 120 logical units, one
 * second of scroll, then names a biome. A tick steps the stage then scrolls:
 * at the step of tick 61 the level scrolled 60 ticks, just short of a second,
 * so the stage goes on at tick 62. */
constexpr std::string_view WAITS_FOR_A_SECOND_OF_SCROLL = R"(
level.register("test_level", { function()
  level.biome("near")
  level.wait_scroll(120)
  level.biome("far")
end })
)";
constexpr std::string_view BIOME_BEFORE_THE_SCROLL = "near";
constexpr std::string_view BIOME_AFTER_THE_SCROLL = "far";
constexpr std::size_t TICKS_JUST_SHORT_OF_A_SECOND_OF_SCROLL = 61;

/** @brief A level whose stage pauses the scroll, then waits for a place the
 * paused scroll never reaches. */
constexpr std::string_view PAUSES_THEN_WAITS_FOR_SCROLL = R"(
level.register("test_level", { function()
  level.pause_scroll()
  level.biome("near")
  level.wait_scroll(120)
  level.biome("far")
end })
)";
constexpr std::size_t TICKS_OF_TWO_SECONDS = 120;
constexpr float START_OF_THE_SCROLL = 0.0F;

/** @brief A level whose stage pauses the scroll for 50 ms: the scroll moves
 * again from tick 4, whose scroll is the first, 2 logical units. */
constexpr std::string_view PAUSES_FOR_A_WAIT = R"(
level.register("test_level", { function()
  level.pause_scroll()
  level.wait(50)
  level.resume_scroll()
end })
)";
constexpr float UNITS_SCROLLED_IN_ONE_TICK = 2.0F;
constexpr float SCROLL_TOLERANCE = 0.001F;

/** @brief A level whose stage names a biome once the level has scrolled. */
constexpr std::string_view READS_THE_SCROLL = R"(
level.register("test_level", { function()
  level.wait(50)
  if level.scroll() > 0 then
    level.biome("moved")
  end
end })
)";
constexpr std::string_view BIOME_OF_A_LEVEL_THAT_MOVED = "moved";

/** @brief A level whose stage names a biome when the world holds one
 * enemy. */
constexpr std::string_view COUNTS_THE_ENEMIES = R"(
level.register("test_level", { function()
  if level.enemies() == 1 then
    level.biome("one_enemy")
  end
end })
)";
constexpr std::string_view BIOME_OF_ONE_ENEMY = "one_enemy";

/** @brief A level whose stage names a biome when the world holds one boss,
 * whatever enemies it holds besides. */
constexpr std::string_view COUNTS_THE_BOSSES = R"(
level.register("test_level", { function()
  if level.bosses() == 1 then
    level.biome("one_boss")
  end
end })
)";
constexpr std::string_view BIOME_OF_ONE_BOSS = "one_boss";

/** @brief A level of two stages. The first names a biome and waits 50 ms: it
 * ends at tick 4, after 3 ticks of scroll, where the checkpoint is. The
 * second, which starts at tick 5, names a biome, pauses the scroll, waits
 * 50 ms and names another, at tick 8. */
constexpr std::string_view TWO_STAGES = R"(
level.register("test_level", {
  function()
    level.biome("first")
    level.wait(50)
  end,
  function()
    level.biome("second")
    level.pause_scroll()
    level.wait(50)
    level.biome("late")
  end,
})
)";
constexpr std::string_view BIOME_OF_THE_FIRST_STAGE = "first";
constexpr std::string_view BIOME_OF_THE_SECOND_STAGE = "second";
constexpr std::string_view LATE_BIOME_OF_THE_SECOND_STAGE = "late";
constexpr std::size_t TICKS_INTO_THE_FIRST_STAGE = 2;
constexpr std::size_t TICKS_UNTIL_THE_FIRST_STAGE_ENDS = 4;
constexpr std::size_t TICKS_UNTIL_THE_SECOND_STAGE_STARTS = 5;
constexpr std::size_t TICKS_UNTIL_THE_SECOND_STAGE_IS_LATE = 8;
constexpr std::size_t SECOND_STAGE = 1;
constexpr rtype::engine::Duration SCROLL_AT_THE_CHECKPOINT =
    3 * rtype::engine::SIMULATION_TICK_DURATION;
constexpr float SPEED_OF_A_MOVING_LEVEL = 120.0F;

/** @brief A level whose stage names a biome, waits until it scrolled 120
 * logical units and names it again: it still began at the start of the
 * scroll. The second biome of WAITS_FOR_A_SECOND_OF_SCROLL is named at the
 * step of tick 62, after 61 ticks of 2 units: 122 units. */
constexpr std::string_view NAMES_A_BIOME_AGAIN = R"(
level.register("test_level", { function()
  level.biome("near")
  level.wait_scroll(120)
  level.biome("near")
end })
)";
constexpr float WHERE_THE_FIRST_BIOME_BEGAN = 0.0F;
constexpr float WHERE_THE_BIOME_AFTER_THE_SCROLL_BEGAN = 122.0F;

/** @brief A level of two stages whose first names its biome after a wait of
 * 50 ms, at the step of tick 4, after 3 ticks of 2 units: 6 units. The
 * second names another at tick 5, 8 units in, and pauses the scroll. */
constexpr std::string_view TWO_STAGES_WITH_A_LATE_BIOME = R"(
level.register("test_level", {
  function()
    level.wait(50)
    level.biome("first")
  end,
  function()
    level.biome("second")
    level.pause_scroll()
    level.wait(50)
  end,
})
)";
constexpr float WHERE_THE_LATE_FIRST_BIOME_BEGAN = 6.0F;

/** @brief A level whose stage fails, and what its error says. */
constexpr std::string_view FAILS = R"(
level.register("test_level", { function()
  error("the stage broke")
end })
)";
constexpr std::string_view FAILURE_OF_THE_STAGE = "the stage broke";

/** @brief A level whose stage yields without a wait of `level`. */
constexpr std::string_view YIELDS_BY_ITSELF = R"(
level.register("test_level", { function()
  coroutine.yield()
end })
)";

/** @brief A level whose stage gives a biome a name no biome can have. */
constexpr std::string_view NAMES_A_WRONG_BIOME = R"(
level.register("test_level", { function()
  level.biome("Not A Biome")
end })
)";

/** @brief A level whose stage fails after naming a biome and waiting. */
constexpr std::string_view FAILS_AFTER_A_WAIT = R"(
level.register("test_level", { function()
  level.biome("before")
  level.wait(50)
  error("the stage broke")
end })
)";

/** @brief The enemies the tests place: a Drifter then a Weaver, and a Drifter
 * with a field `count` of 3. */
constexpr std::string_view WEAVER_SHEET = "weaver";
constexpr rtype::game::Position WHERE_THE_DRIFTER_IS{.x = 2224.0F, .y = 300.0F};
constexpr rtype::game::Position WHERE_THE_WEAVER_IS{.x = 2704.0F, .y = 400.0F};
constexpr std::string_view COUNT_FIELD_NAME = "count";
constexpr double COUNT_FIELD_VALUE = 3.0;

/** @brief A level whose stage names as its biome the sheet of the first
 * placed enemy. */
constexpr std::string_view NAMES_THE_FIRST_PLACED_SHEET = R"(
level.register("test_level", { function()
  level.biome(level.placements()[1].sheet)
end })
)";

/** @brief A level whose stage names the biome `three` when the field count
 * of the first placed enemy is 3, `other` otherwise. */
constexpr std::string_view READS_THE_COUNT_FIELD = R"(
level.register("test_level", { function()
  if level.placements()[1].fields.count == 3 then
    level.biome("three")
  else
    level.biome("other")
  end
end })
)";
constexpr std::string_view THREE_BIOME = "three";

/** @brief A level whose stage asks for the placed enemies, then waits for a
 * long time. */
constexpr std::string_view ASKS_FOR_THE_PLACED_ENEMIES = R"(
level.register("test_level", { function()
  level.spawn_placed()
  level.wait(60000)
end })
)";

/** @brief A level of two stages: the first asks for the placed enemies and
 * ends, so the checkpoint is at tick 2; the second waits for a long time. */
constexpr std::string_view ASKS_THEN_REACHES_A_CHECKPOINT = R"(
level.register("test_level", {
  function()
    level.spawn_placed()
  end,
  function()
    level.wait(60000)
  end,
})
)";
constexpr std::size_t TICKS_TO_THE_SECOND_STAGE = 2;

/** @brief A level whose stage names a biome, plays a transition of 3 s, the
 * shortest, then names another; and one whose transition is a millisecond
 * shorter than that. A transition of 3000 ms lasts 180 ticks: a stage that
 * starts it at tick 1 goes on at tick 181. */
constexpr std::string_view PLAYS_A_TRANSITION = R"(
level.register("test_level", { function()
  level.biome("before")
  level.transition(3000)
  level.biome("after")
end })
)";
constexpr std::string_view PLAYS_A_SHORT_TRANSITION = R"(
level.register("test_level", { function()
  level.transition(2999)
end })
)";
constexpr std::uint32_t TICKS_OF_THE_TRANSITION = 180;
constexpr std::size_t TICKS_UNTIL_THE_LAST_OF_THE_TRANSITION = 180;
constexpr std::size_t TICKS_UNTIL_THE_TRANSITION_IS_OVER = 181;
constexpr std::string_view BIOME_BEFORE_THE_TRANSITION = "before";
constexpr std::string_view BIOME_AFTER_THE_TRANSITION = "after";
constexpr std::string_view WORD_OF_A_REFUSED_TRANSITION = "transition";

/** @brief The ground of the biome caves, one map one pixel wide: its floor
 * at row 200, its ceiling at row 40. */
constexpr std::string_view GROUND_BIOME = "caves";
constexpr int CAVES_FLOOR_ROW = 200;
constexpr int CAVES_CEILING_ROW = 40;

/** @brief A level whose stage enters the caves, then reads the ground: at
 * the right edge, where the caves begin, the floor at 800 units and the
 * ceiling at 160, 4 units a pixel; at the left edge, before them, none. It
 * names the biome found when every answer is right, lost otherwise. */
constexpr std::string_view READS_THE_GROUND = R"(
level.register("test_level", { function()
  level.biome("caves")
  if level.ground(1920) == 800 and level.ceiling(1920) == 160 and level.ground(0) == nil then
    level.biome("found")
  else
    level.biome("lost")
  end
end })
)";
constexpr std::string_view GROUND_FOUND_BIOME = "found";

/** @brief A level whose stage stays in the open and reads the ground: it
 * names the biome none when there is none. */
constexpr std::string_view READS_NO_GROUND = R"(
level.register("test_level", { function()
  if level.ground(1920) == nil and level.ceiling(1920) == nil then
    level.biome("none")
  end
end })
)";
constexpr std::string_view NO_GROUND_BIOME = "none";
