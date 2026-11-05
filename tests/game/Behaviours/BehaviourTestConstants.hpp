#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

/** @brief The row a piloted body of the tests appears on, and where along it,
 * in logical units. */
constexpr float ROW_Y = 300.0F;
constexpr float BODY_START_X = 800.0F;
constexpr Position ROW_START{.x = BODY_START_X, .y = ROW_Y};

/** @brief The top speed of a Bydo, which the requirements set at 240 logical
 * units per second. */
constexpr float BODY_TOP_SPEED = 240.0F;

/** @brief A quarter and a half of the 2 second period of a weave. */
constexpr engine::Duration QUARTER_PERIOD = std::chrono::milliseconds(500);
constexpr engine::Duration HALF_PERIOD = std::chrono::seconds(1);

/** @brief Where a body on the row at 300 is at the bottom of its weave: 48
 * units lower, as y grows downwards. */
constexpr float WAVE_BOTTOM_Y = 348.0F;

/** @brief The vertical share that takes a body at 240 units per second from
 * its row to the bottom of its weave, 48 units lower, in a quarter period of
 * half a second: 48 / (240 * 0.5). */
constexpr float QUARTER_PERIOD_VERTICAL_SHARE = 0.4F;

constexpr float SHARE_TOLERANCE = 1e-4F;

/** @brief The values a sheet gives the settings of a behaviour, as the
 * requirements set them: a Drifter heads left, a missile of a player right, a
 * Weaver weaves 48 units every 2000 ms; a weave half as deep for the tests
 * that change a setting. */
constexpr std::int64_t LEFTWARD_HEADING = -1;
constexpr std::int64_t RIGHTWARD_HEADING = 1;
constexpr std::int64_t WEAVER_AMPLITUDE_UNITS = 48;
constexpr std::int64_t HALF_WEAVER_AMPLITUDE_UNITS = 24;
constexpr std::int64_t WEAVER_PERIOD_MS = 2000;
constexpr std::array LEFT_HEADING_SETTINGS{LEFTWARD_HEADING};
constexpr std::array RIGHT_HEADING_SETTINGS{RIGHTWARD_HEADING};
constexpr std::array WEAVER_SETTINGS{WEAVER_AMPLITUDE_UNITS, WEAVER_PERIOD_MS};
constexpr std::array HALF_DEPTH_WEAVE_SETTINGS{HALF_WEAVER_AMPLITUDE_UNITS,
                                               WEAVER_PERIOD_MS};

/** @brief The vertical share that takes a body at 240 units per second to the
 * bottom of a weave 24 units deep in half a second: 24 / (240 * 0.5). */
constexpr float HALF_DEPTH_QUARTER_PERIOD_VERTICAL_SHARE = 0.2F;

/** @brief What a sheet calls the settings, and what each accepts, as the
 * Entity types section of the architecture page sets them. */
constexpr std::string_view HEADING_KEY_IN_A_SHEET = "heading";
constexpr std::int64_t LOWEST_HEADING = -1;
constexpr std::int64_t HIGHEST_HEADING = 1;
constexpr std::string_view AMPLITUDE_KEY_IN_A_SHEET = "amplitude";
constexpr std::string_view PERIOD_KEY_IN_A_SHEET = "period_ms";
constexpr std::int64_t LOWEST_AMPLITUDE = 0;
constexpr std::int64_t HIGHEST_AMPLITUDE = 48;
constexpr std::int64_t LOWEST_PERIOD_MS = 1;
constexpr std::int64_t HIGHEST_PERIOD_MS = 60000;

/** @brief Where the second setting of a body is, and a place past it. */
constexpr std::size_t SECOND_SETTING = 1;
constexpr std::size_t THIRD_SETTING = 2;

}  // namespace rtype::game::testing
