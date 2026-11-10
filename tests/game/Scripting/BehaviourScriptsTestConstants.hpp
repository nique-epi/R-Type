#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "Position.hpp"

/** @brief A script that registers `charge`: it goes left at the share of its
 * speed its setting gives, in percent, up or down towards the nearest ship,
 * fires when it was hit, and starts the strike of its attack 1. */
constexpr std::string_view CHARGE_SCRIPT = R"(
behaviours.register("charge", {
  { name = "speed_share", lowest = 0, highest = 100 },
}, function(view)
  local share = view.settings.speed_share / 100
  if view.nearest_ship == nil then
    return { direction = { x = -share, y = 0 } }
  end
  local down = view.nearest_ship.y > view.position.y
  return {
    direction = { x = -share, y = down and 1 or -1 },
    shooting = view.wounded,
    starts_action = { attack = 1, phase = "strike" },
  }
end)
)";

/** @brief The name, setting and range `charge` declares. */
constexpr std::string_view CHARGE_BEHAVIOUR = "charge";
constexpr std::string_view SPEED_SHARE_SETTING = "speed_share";
constexpr std::int64_t LOWEST_SPEED_SHARE = 0;
constexpr std::int64_t HIGHEST_SPEED_SHARE = 100;
constexpr std::size_t SETTINGS_OF_CHARGE = 1;

/** @brief A body at half its speed, in the middle of the playfield, a ship
 * below and to its left, and what `charge` makes of it. */
constexpr std::array<std::int64_t, 1> HALF_SPEED_SETTINGS{50};
constexpr rtype::game::Position BODY_IN_THE_MIDDLE{.x = 1000.0F, .y = 500.0F};
constexpr rtype::game::Position SHIP_BELOW{.x = 100.0F, .y = 800.0F};
constexpr float HALF_LEFTWARD = -0.5F;
constexpr float FULL_DOWNWARD = 1.0F;
constexpr std::uint8_t CHARGE_ATTACK = 1;

/** @brief A script whose behaviour asks five times its full speed. */
constexpr std::string_view TOO_FAST_SCRIPT = R"(
behaviours.register("too_fast", {}, function(view)
  return { direction = { x = -5, y = 7 } }
end)
)";
constexpr std::string_view TOO_FAST_BEHAVIOUR = "too_fast";
constexpr float FULL_LEFTWARD = -1.0F;

/** @brief A script whose behaviour always fails. */
constexpr std::string_view FAILING_SCRIPT = R"(
behaviours.register("broken", {}, function(view)
  error("the steering of broken failed")
end)
)";
constexpr std::string_view FAILING_BEHAVIOUR = "broken";
constexpr std::size_t REPORTED_ONCE = 1;

/** @brief A script that takes the name of a built-in behaviour. */
constexpr std::string_view STEALS_A_BUILT_IN_NAME = R"(
behaviours.register("straight", {}, function(view) return {} end)
)";

/** @brief A script that registers a behaviour without its function. */
constexpr std::string_view REGISTERS_WITHOUT_FUNCTION =
    R"(behaviours.register("lost", {}))";

/** @brief The name of the scripts of these tests. */
constexpr std::string_view TEST_SCRIPT_NAME = "test/server/test.luau";

/** @brief A script whose behaviour, under half its health, grows its box and
 * doubles its speed. */
constexpr std::string_view ENRAGING_SCRIPT = R"(
behaviours.register("enraging", {}, function(view)
  if view.health_share < 0.5 then
    return {
      collision_box = { width = 200, height = 100 },
      top_speed = 480,
    }
  end
  return {}
end)
)";
constexpr std::string_view ENRAGING_BEHAVIOUR = "enraging";
constexpr float A_QUARTER_OF_ITS_HEALTH = 0.25F;
constexpr float ALL_ITS_HEALTH = 1.0F;
constexpr float ENRAGED_WIDTH = 200.0F;
constexpr float ENRAGED_HEIGHT = 100.0F;
constexpr float ENRAGED_TOP_SPEED = 480.0F;

/** @brief A script whose behaviour fires while its boss enraged, and takes
 * as top speed the number of actions it ended. */
constexpr std::string_view BOSS_READING_SCRIPT = R"(
behaviours.register("boss_reader", {}, function(view)
  return { shooting = view.enraged, top_speed = view.actions_ended }
end)
)";
constexpr std::string_view BOSS_READING_BEHAVIOUR = "boss_reader";
constexpr std::uint32_t THREE_ENDED_ACTIONS = 3;
constexpr float THREE_UNITS_PER_SECOND = 3.0F;

/** @brief The example resource the repository ships, the behaviour its
 * script registers, and the settings its sheet gives: within 96 units of its
 * row, cruising at 360 units per second, charging at 720. */
constexpr std::string_view EXAMPLE_RESOURCE_NAME = "crimson-drifter";
constexpr std::string_view EXAMPLE_BEHAVIOUR = "crimson_charge";
constexpr std::array<std::int64_t, 3> EXAMPLE_SETTINGS{96, 360, 720};
constexpr float EXAMPLE_CRUISE_SPEED = 360.0F;
constexpr float EXAMPLE_CHARGE_SPEED = 720.0F;

/** @brief A ship level with the body, and a ship far below it. */
constexpr rtype::game::Position SHIP_LEVEL{.x = 100.0F, .y = 540.0F};
constexpr rtype::game::Position SHIP_FAR_BELOW{.x = 100.0F, .y = 900.0F};
