#pragma once

#include <cstdint>
#include "Position.hpp"
#include "ProtocolConstants.hpp"
#include "WorldState.hpp"

namespace rtype::client::testing {

/**
 * @brief The network ids of the entities the tests announce, one per type, and
 * an id no message announces.
 */
constexpr std::uint32_t SHIP_ID = 7;
constexpr std::uint32_t PLAYER_MISSILE_ID = 8;
constexpr std::uint32_t BYDO_ID = 9;
constexpr std::uint32_t ENEMY_MISSILE_ID = 10;
constexpr std::uint32_t UNANNOUNCED_ID = 11;

/**
 * @brief Where a message first puts an entity in the tests, then where a later
 * message puts it, in logical units.
 */
constexpr game::Position FIRST_POSITION{.x = 100.0F, .y = 200.0F};
constexpr game::Position SECOND_POSITION{.x = 150.5F, .y = 210.25F};

/**
 * @brief The positions of the four entities, one per type, of the test that
 * announces them all: each differs from the others, so an entity drawn with
 * another type's sprite is seen.
 */
constexpr game::Position SHIP_POSITION{.x = 10.0F, .y = 20.0F};
constexpr game::Position PLAYER_MISSILE_POSITION{.x = 30.0F, .y = 40.0F};
constexpr game::Position BYDO_POSITION{.x = 50.0F, .y = 60.0F};
constexpr game::Position ENEMY_MISSILE_POSITION{.x = 70.0F, .y = 80.0F};

/**
 * @brief The values just outside the entity types of the protocol: the one
 * before the first and the one after the last.
 */
constexpr auto WIRE_TYPE_BEFORE_THE_FIRST =
    static_cast<network::WireEntityType>(network::FIRST_WIRE_ENTITY_TYPE - 1);
constexpr auto WIRE_TYPE_AFTER_THE_LAST =
    static_cast<network::WireEntityType>(network::LAST_WIRE_ENTITY_TYPE + 1);

}  // namespace rtype::client::testing
