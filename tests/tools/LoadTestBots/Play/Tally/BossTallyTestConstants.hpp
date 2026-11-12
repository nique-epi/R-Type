#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>
#include "SightFigures.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools::testing {

/** @brief The entity id the boss of the tests has, and its full health. */
constexpr std::uint32_t BOSS_ENTITY_ID = 7;
constexpr std::uint16_t FULL_BOSS_HEALTH = 400;

/** @brief Health left, in points, and what it reads in percent of
 * FULL_BOSS_HEALTH: 300 of 400 is 75 %, and the single point of a boss about
 * to die is rounded up to 1 %. */
constexpr std::uint16_t THREE_QUARTERS_OF_THE_HEALTH = 300;
constexpr std::uint8_t THREE_QUARTERS_PERCENT = 75;
constexpr std::uint16_t LAST_HEALTH_POINT = 1;
constexpr std::uint8_t LAST_HEALTH_POINT_PERCENT = 1;

/** @brief A second of frames, and half of it. */
constexpr engine::Duration A_SECOND = std::chrono::seconds{1};
constexpr engine::Duration HALF_A_SECOND = std::chrono::milliseconds{500};
constexpr engine::Duration TWO_SECONDS = std::chrono::seconds{2};

/** @brief The time of a reading taken between two messages of one frame. */
constexpr engine::Duration NO_TIME{};

/** @brief What a bot knows of its run: a life left with none lost yet, a life
 * left after two were lost, and no life left after three were lost. */
constexpr SightFigures ALIVE_NOTHING_LOST{
    .tick = std::nullopt, .inPlay = true, .livesLost = 0};
constexpr SightFigures ALIVE_TWO_LOST{
    .tick = std::nullopt, .inPlay = true, .livesLost = 2};
constexpr SightFigures DEAD_THREE_LOST{
    .tick = std::nullopt, .inPlay = false, .livesLost = 3};

constexpr std::size_t TWO_FIGHTS = 2;
constexpr std::uint32_t TWO_LIVES = 2;

}  // namespace rtype::tools::testing
