#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include "Position.hpp"

namespace rtype::server::testing {

/**
 * @brief The ports of the two clients the tests send to.
 */
constexpr std::uint16_t FIRST_CLIENT_PORT = 4000;
constexpr std::uint16_t SECOND_CLIENT_PORT = 4001;

/**
 * @brief The network id and the position of the ship of the tests that need one
 * entity.
 */
constexpr std::uint32_t SHIP_NETWORK_ID = 5;
constexpr game::Position SHIP_POSITION{.x = 1.0F, .y = 2.0F};

/**
 * @brief The network ids and the positions of the four entities, one of each
 * type, of the test that lists them.
 */
constexpr std::uint32_t PLAYER_NETWORK_ID = 1;
constexpr std::uint32_t ENEMY_NETWORK_ID = 2;
constexpr std::uint32_t PLAYER_MISSILE_NETWORK_ID = 3;
constexpr std::uint32_t ENEMY_MISSILE_NETWORK_ID = 4;
constexpr game::Position PLAYER_POSITION{.x = 10.0F, .y = 20.0F};
constexpr game::Position ENEMY_POSITION{.x = 11.0F, .y = 21.0F};
constexpr game::Position PLAYER_MISSILE_POSITION{.x = 12.0F, .y = 22.0F};
constexpr game::Position ENEMY_MISSILE_POSITION{.x = 13.0F, .y = 23.0F};

/**
 * @brief The ships and the scores of the two players of the test that lists
 * them.
 */
constexpr std::uint32_t FIRST_SHIP_ID = 11;
constexpr std::uint32_t SECOND_SHIP_ID = 12;
constexpr std::uint32_t FIRST_SCORE = 300;
constexpr std::uint32_t SECOND_SCORE = 100;

/**
 * @brief The lives of the two players of the test that lists them, different
 * so that two swapped players fail it.
 */
constexpr std::uint8_t FIRST_LIVES = 2;
constexpr std::uint8_t SECOND_LIVES = 1;

/**
 * @brief The protection a ship has left on the last tick it cannot be hit.
 */
constexpr std::size_t ONE_TICK_OF_PROTECTION = 1;

/**
 * @brief The network id of an entity that has no type.
 */
constexpr std::uint32_t UNTYPED_NETWORK_ID = 99;

/**
 * @brief Positions a game bug could give an entity: a coordinate that is not a
 * number, and one that is infinite.
 */
constexpr game::Position NOT_A_NUMBER_POSITION{
    .x = std::numeric_limits<float>::quiet_NaN(), .y = 0.0F};
constexpr game::Position INFINITE_POSITION{
    .x = 0.0F, .y = std::numeric_limits<float>::infinity()};

/**
 * @brief More entities than one datagram holds.
 */
constexpr std::size_t MANY_ENTITIES = 300;

}  // namespace rtype::server::testing
