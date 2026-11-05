#pragma once

#include <cstdint>

namespace rtype::server::testing {

/**
 * @brief Type numbers the tests of the replication give their entities. Only
 * their being distinct matters: a sender copies the number of an entity onto
 * the wire as it is. They are the numbers the sheets the game ships with get,
 * in the order of their names, so a failure reads like a game.
 */
constexpr std::uint8_t DRIFTER_TYPE_NUMBER = 1;
constexpr std::uint8_t ENEMY_MISSILE_TYPE_NUMBER = 2;
constexpr std::uint8_t PLAYER_MISSILE_TYPE_NUMBER = 3;
constexpr std::uint8_t SHIP_TYPE_NUMBER = 4;

}  // namespace rtype::server::testing
