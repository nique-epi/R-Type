#pragma once

#include <cstdint>

namespace rtype::client::testing {

/**
 * @brief The numbers the server gives the five sheets it ships, in the order
 * of their names, from 1: drifter, enemy_missile, player_missile, ship,
 * weaver; then the first number past them, which names no type.
 */
constexpr std::uint8_t DRIFTER_TYPE_NUMBER = 1;
constexpr std::uint8_t ENEMY_MISSILE_TYPE_NUMBER = 2;
constexpr std::uint8_t PLAYER_MISSILE_TYPE_NUMBER = 3;
constexpr std::uint8_t SHIP_TYPE_NUMBER = 4;
constexpr std::uint8_t WEAVER_TYPE_NUMBER = 5;
constexpr std::uint8_t NUMBER_OF_NO_TYPE = 6;

}  // namespace rtype::client::testing
