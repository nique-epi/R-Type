#pragma once

#include <cstdint>

namespace rtype::tools::testing {

constexpr std::uint8_t SIGHT_PLAYER = 1;
constexpr std::uint32_t SIGHT_SHIP_ID = 100;
constexpr std::uint32_t SEEN_ID = 7;

/** @brief The numbers FixedEntityTypeTable gives its types. */
constexpr std::uint8_t ENEMY_TYPE = 1;
constexpr std::uint8_t ENEMY_MISSILE_TYPE = 2;
constexpr std::uint8_t PLAYER_MISSILE_TYPE = 4;
constexpr std::uint8_t SHIP_TYPE = 5;
constexpr std::uint8_t BOSS_TYPE = 6;
constexpr std::uint8_t NO_SUCH_TYPE = 99;

/** @brief Three ticks are 1/20 s, so 12 units in them are 240 per second. */
constexpr std::uint32_t SIGHT_FIRST_TICK = 30;
constexpr std::uint32_t THREE_TICKS_LATER = 33;
constexpr float START_X = 500.0F;
constexpr float TWELVE_UNITS_LEFT = 488.0F;
constexpr float SPEED_TO_THE_LEFT = -240.0F;

constexpr float SHIP_X = 960.0F;
constexpr float SHIP_Y = 540.0F;
constexpr std::uint8_t THREE_LIVES = 3;
constexpr std::uint8_t TWO_LIVES = 2;
constexpr std::uint8_t NO_LIFE = 0;
constexpr std::uint32_t TWO_HUNDRED_POINTS = 200;
constexpr std::uint32_t THREE_HUNDRED_POINTS = 300;

}  // namespace rtype::tools::testing
