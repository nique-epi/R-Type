#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include "CollisionBox.hpp"
#include "Health.hpp"

/** @brief The health of the bodies of these tests: 3 points of 5. */
constexpr rtype::game::Health THREE_OF_FIVE_POINTS{.points = 3, .full = 5};

/** @brief The behaviour of the scripts of these tests. */
constexpr std::string_view FIELD_BEHAVIOUR = "fields";

/** @brief A script that asks for a top speed of a hundred times the health of
 * its body, and its speed at 3 points. */
constexpr std::string_view HEALTH_READING_SCRIPT = R"(
behaviours.register("fields", {}, function(e)
  return { top_speed = e.health * 100 }
end)
)";
constexpr float SPEED_AT_THREE_POINTS = 300.0F;

/** @brief A script that asks for a top speed of a hundred times the full
 * health of its body, and its speed at 5 full points. */
constexpr std::string_view FULL_HEALTH_READING_SCRIPT = R"(
behaviours.register("fields", {}, function(e)
  return { top_speed = e.full_health * 100 }
end)
)";
constexpr float SPEED_AT_FIVE_FULL_POINTS = 500.0F;

/** @brief A script that fires when its body has no gun interval to read. */
constexpr std::string_view GUN_ABSENCE_READING_SCRIPT = R"(
behaviours.register("fields", {}, function(e)
  return { shooting = e.gun_interval == nil }
end)
)";

/** @brief A script that sets the health of its body to 2 points. */
constexpr std::string_view HEALTH_WRITING_SCRIPT = R"(
behaviours.register("fields", {}, function(e)
  e.health = 2
  return {}
end)
)";
constexpr int TWO_POINTS = 2;

/** @brief A script that moves its body to (10, 20). */
constexpr std::string_view POSITION_WRITING_SCRIPT = R"(
behaviours.register("fields", {}, function(e)
  e.position = { x = 10, y = 20 }
  return {}
end)
)";
constexpr float WRITTEN_X = 10.0F;
constexpr float WRITTEN_Y = 20.0F;

/** @brief A box laid off the center of its body, as a boss sheet gives one,
 * and a script that makes it 40 by 30. */
constexpr rtype::game::CollisionBox BOX_OFF_CENTER{
    .width = 100.0F, .height = 50.0F, .offsetX = 12.0F, .offsetY = -8.0F};
constexpr std::string_view BOX_WRITING_SCRIPT = R"(
behaviours.register("fields", {}, function(e)
  e.collision_box = { width = 40, height = 30 }
  return {}
end)
)";
constexpr float WRITTEN_WIDTH = 40.0F;
constexpr float WRITTEN_HEIGHT = 30.0F;

/** @brief A script that sets the top speed of its body to 250. */
constexpr std::string_view TOP_SPEED_WRITING_SCRIPT = R"(
behaviours.register("fields", {}, function(e)
  e.top_speed = 250
  return {}
end)
)";
constexpr float WRITTEN_TOP_SPEED = 250.0F;

/** @brief A script that makes its body worth 700 points. */
constexpr std::string_view SCORE_WRITING_SCRIPT = R"(
behaviours.register("fields", {}, function(e)
  e.score = 700
  return {}
end)
)";
constexpr std::uint32_t WRITTEN_SCORE = 700;

/** @brief A script that makes its gun fire every 12 ticks. */
constexpr std::string_view GUN_INTERVAL_WRITING_SCRIPT = R"(
behaviours.register("fields", {}, function(e)
  e.gun_interval = 12
  return {}
end)
)";
constexpr std::size_t WRITTEN_GUN_INTERVAL = 12;

/** @brief Scripts that write a value a component refuses, or a field no
 * script may write. */
constexpr std::string_view NO_HEALTH_WRITING_SCRIPT = R"(
behaviours.register("fields", {}, function(e)
  e.health = 0
  return {}
end)
)";
constexpr std::string_view HEALTH_ABOVE_FULL_WRITING_SCRIPT = R"(
behaviours.register("fields", {}, function(e)
  e.health = 6
  return {}
end)
)";
constexpr std::string_view INFINITE_POSITION_WRITING_SCRIPT = R"(
behaviours.register("fields", {}, function(e)
  e.position = { x = math.huge, y = 20 }
  return {}
end)
)";
constexpr std::string_view NEGATIVE_BOX_WRITING_SCRIPT = R"(
behaviours.register("fields", {}, function(e)
  e.collision_box = { width = -40, height = 30 }
  return {}
end)
)";
constexpr std::string_view HALF_SCORE_WRITING_SCRIPT = R"(
behaviours.register("fields", {}, function(e)
  e.score = 1.5
  return {}
end)
)";
constexpr std::string_view FULL_HEALTH_WRITING_SCRIPT = R"(
behaviours.register("fields", {}, function(e)
  e.full_health = 9
  return {}
end)
)";
constexpr std::string_view LIVES_WRITING_SCRIPT = R"(
behaviours.register("fields", {}, function(e)
  e.lives = 3
  return {}
end)
)";

/** @brief What the failures of those scripts say. */
constexpr std::string_view ABSENT_GUN_FAILURE =
    "e.gun_interval: this body has no such component";
constexpr std::string_view HEALTH_FAILURE =
    "health is a whole number from 1 to full_health";
constexpr std::string_view POSITION_FAILURE =
    "a position is a table of two finite numbers, x and y";
constexpr std::string_view BOX_FAILURE =
    "a collision box is a table of two positive finite numbers";
constexpr std::string_view SCORE_FAILURE =
    "a score is a whole number from 0 to 4294967295";
constexpr std::string_view FULL_HEALTH_FAILURE =
    "e.full_health cannot be written";
constexpr std::string_view LIVES_FAILURE = "e.lives cannot be written";

/** @brief The line of the scripts above that writes the field. */
constexpr std::string_view WRITING_LINE = "test/server/test.luau:3:";
