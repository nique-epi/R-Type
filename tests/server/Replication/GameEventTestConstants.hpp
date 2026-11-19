#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <string_view>
#include "PlayBand.hpp"

namespace rtype::server::testing {

constexpr std::uint32_t MONSTER_NETWORK_ID = 7;

/** @brief An effect a sheet names. */
constexpr std::string_view SPARK_EFFECT = "spark";
constexpr float MONSTER_X = 100.0F;
constexpr float MONSTER_Y = 50.0F;
constexpr std::uint8_t SECOND_PLAYER_NUMBER = 2;

/** @brief The attack of an enemy the enemy action tests announce. */
constexpr std::uint8_t SOME_ATTACK_NUMBER = 3;

/**
 * @brief The largest event number: the next one wraps to 0.
 */
constexpr std::uint16_t LARGEST_EVENT_NUMBER = 65535;

/**
 * @brief Events that carry the number 1 to LARGEST_EVENT_NUMBER, then one more
 * that wraps to 0, then one more that carries 1 again.
 */
constexpr std::size_t EVENTS_PAST_THE_WRAP =
    std::size_t{LARGEST_EVENT_NUMBER} + 2;

/**
 * @brief Values the protocol refuses: a coordinate that is not a number, the
 * network id no entity has, and a player number outside 1 to 4.
 */
constexpr float NOT_A_NUMBER = std::numeric_limits<float>::quiet_NaN();
constexpr std::uint32_t UNASSIGNED_NETWORK_ID = 0;
constexpr std::uint8_t PLAYER_NUMBER_OUTSIDE_THE_GAME = 0;

/** @brief The boss of the encoder tests: network id 31, sheet 3, 200 of its
 * 240 points left, and the warning of its arrival. */
constexpr std::uint32_t BOSS_NETWORK_ID = 31;
constexpr std::uint8_t BOSS_TYPE_NUMBER = 3;
constexpr std::uint16_t BOSS_POINTS_LEFT = 200;
constexpr std::uint16_t BOSS_FULL_POINTS = 240;
constexpr std::uint32_t BOSS_WARNING_MILLISECONDS = 4400;

/** @brief A level in the biome wreck, scrolled 240 logical units, whose
 * scroll is paused. */
constexpr std::string_view LEVEL_BIOME = "wreck";
constexpr float LEVEL_SCROLL_POSITION = 240.0F;
constexpr float PAUSED_LEVEL_SCROLL_SPEED = 0.0F;

/** @brief The band of that level, its ceiling on row 192 and its floor on row
 * 848, as the level holds it and as the wire writes it. */
constexpr game::PlayBand LEVEL_BAND{.top = 192.0F, .bottom = 848.0F};
constexpr std::uint16_t WIRE_LEVEL_BAND_TOP = 192;
constexpr std::uint16_t WIRE_LEVEL_BAND_BOTTOM = 848;

/** @brief Where the scroll stood when the biome a level opens in began: 4096
 * logical units before its start. */
constexpr float OPENING_BIOME_BEGAN_AT = -4096.0F;

}  // namespace rtype::server::testing
