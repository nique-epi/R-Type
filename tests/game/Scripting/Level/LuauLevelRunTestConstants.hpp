#pragma once

#include <cstddef>
#include <string_view>
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

/** @brief The sheet the bosses of these tests know as a boss. */
constexpr std::string_view BOSS_OF_THE_TESTS = "tyrant";

/** @brief A level whose stage names a biome, brings the boss in with 40 % of
 * the health of its sheet, then names another biome once the boss entered. */
constexpr std::string_view BRINGS_A_BOSS_IN = R"(
level.register("test_level", { function()
  level.biome("before")
  level.boss("tyrant", { health_percent = 40 })
  level.biome("after")
end })
)";
constexpr int HEALTH_PERCENT_OF_THE_BOSS = 40;
constexpr std::size_t TICKS_THE_BOSS_IS_AWAITED = 10;

/** @brief A level whose stage brings the boss in without options: it has
 * the whole health of its sheet. */
constexpr std::string_view BRINGS_A_WHOLE_BOSS_IN = R"(
level.register("test_level", { function()
  level.boss("tyrant")
end })
)";
constexpr int WHOLE_HEALTH_PERCENT = 100;

/** @brief Levels whose stage brings in a sheet that is no boss, a boss
 * without health, and a boss with more than the health of its sheet. */
constexpr std::string_view BRINGS_IN_A_SHEET_THAT_IS_NO_BOSS = R"(
level.register("test_level", { function()
  level.boss("drifter")
end })
)";
constexpr std::string_view BRINGS_IN_A_BOSS_WITHOUT_HEALTH = R"(
level.register("test_level", { function()
  level.boss("tyrant", { health_percent = 0 })
end })
)";
constexpr std::string_view BRINGS_IN_A_BOSS_ABOVE_ITS_HEALTH = R"(
level.register("test_level", { function()
  level.boss("tyrant", { health_percent = 101 })
end })
)";
