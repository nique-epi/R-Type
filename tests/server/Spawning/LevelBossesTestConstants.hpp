#pragma once

#include <array>
#include <cstddef>
#include "BossStageConstants.hpp"

namespace rtype::server::testing {

/** @brief The health of the warlord of the stage tests, 20 points, taken
 * whole and at half, for one player at most. */
constexpr int WHOLE_WARLORD_PERCENT = 100;
constexpr int HALF_WARLORD_PERCENT = 50;
constexpr int WHOLE_WARLORD_HEALTH = 20;
constexpr int HALF_WARLORD_HEALTH = 10;

/** @brief A difficulty that halves the health of bosses, and a half warlord
 * there: a quarter of its 20 points. */
constexpr int DIFFICULTY_OF_HALF_BOSSES = 50;
constexpr int QUARTER_WARLORD_HEALTH = 5;
constexpr std::array HEALTH_OF_A_QUARTER_WARLORD{QUARTER_WARLORD_HEALTH};
constexpr std::array HEALTHS_OF_A_WHOLE_THEN_A_HALF_WARLORD{
    WHOLE_WARLORD_HEALTH, HALF_WARLORD_HEALTH};

/** @brief A name no sheet of the tests has. */
constexpr const char* NO_SHEET_NAME = "nobody";

/** @brief The ticks of the warning of a boss, after which it is still out,
 * and the tick after them, when it enters. */
constexpr std::size_t TICKS_OF_THE_WARNING = BOSS_ARRIVAL_WARNING_TICKS;
constexpr std::size_t TICKS_UNTIL_THE_BOSS_ENTERS =
    BOSS_ARRIVAL_WARNING_TICKS + 1;

/** @brief Ticks between the announces of two bosses. */
constexpr std::size_t TICKS_BETWEEN_TWO_BOSSES = 5;

/** @brief How many bosses are in the world, or pending. */
constexpr std::size_t NO_BOSS = 0;
constexpr std::size_t ONE_BOSS = 1;
constexpr std::size_t TWO_BOSSES = 2;

}  // namespace rtype::server::testing
