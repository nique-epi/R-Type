#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include "WorldState.hpp"

namespace rtype::network::testing {

/**
 * @brief The tick of the states the tests build.
 */
constexpr std::uint32_t SOME_TICK = 3;

/**
 * @brief A tick late in a long game, far above the first ones.
 */
constexpr std::uint32_t LATE_TICK = 4000000000U;

/**
 * @brief Bytes of the largest payload a datagram carries: 1200, less the 22
 * bytes of the packet header and the 3 of the message prefix.
 */
constexpr std::size_t DATAGRAM_PAYLOAD_LIMIT = 1175;

/**
 * @brief Entities one payload holds next to four players: (1175 - 6 - 4 * 12)
 * / 9, rounded down.
 */
constexpr std::size_t ENTITIES_FITTING_WITH_FOUR_PLAYERS = 124;

/**
 * @brief Entities in a small state.
 */
constexpr std::size_t FEW_ENTITIES = 5;

/**
 * @brief The values of the example of the protocol page: a player of number 1
 * whose ship is entity 7, with 3 lives and 100 points, and an entity 9 at
 * (100, 50.5).
 */
constexpr std::uint8_t EXAMPLE_PLAYER_NUMBER = 1;
constexpr std::uint32_t EXAMPLE_PLAYER_SHIP_ID = 7;
constexpr std::uint8_t SOME_LIVES = 3;
constexpr std::uint32_t SOME_SCORE = 100;
constexpr std::uint32_t EXAMPLE_ENTITY_ID = 9;
constexpr float EXAMPLE_X = 100.0F;
constexpr float EXAMPLE_Y = 50.5F;

/**
 * @brief The height of the entities that someEntities() builds.
 */
constexpr float SOME_ENTITY_Y = 0.5F;

/**
 * @brief A number one past the players a game holds.
 */
constexpr std::uint8_t NUMBER_OF_A_FIFTH_PLAYER = 5;

/**
 * @brief Coordinates that are not a multiple of an eighth of a unit, and the
 * nearest eighth of each.
 */
constexpr float UNROUNDED_X = 0.3F;
constexpr float UNROUNDED_Y = -0.1F;
constexpr float NEAREST_EIGHTH_OF_UNROUNDED_X = 0.25F;
constexpr float NEAREST_EIGHTH_OF_UNROUNDED_Y = -0.125F;

/**
 * @brief Coordinates beyond the range of the wire, and the ends of that range.
 */
constexpr float BEYOND_RANGE_X = 5000.0F;
constexpr float BEYOND_RANGE_Y = -5000.0F;
constexpr float LARGEST_WIRE_COORDINATE = 4095.875F;
constexpr float SMALLEST_WIRE_COORDINATE = -4096.0F;

/**
 * @brief Entity type values that no entity has: one below and one above the
 * values 1 to 4 of the wire.
 */
constexpr unsigned int ENTITY_TYPE_BELOW_THE_FIRST = 0;
constexpr unsigned int ENTITY_TYPE_ABOVE_THE_LAST = 5;

/**
 * @brief The value of the last entity type, EnemyMissile, the upper bound of
 * the types the wire knows.
 */
constexpr unsigned int LAST_ENTITY_TYPE = 4;

/**
 * @brief A byte that is neither 0 nor 1, where a boolean is expected, and the
 * id no entity has.
 */
constexpr unsigned int NOT_A_BOOLEAN = 2;
constexpr unsigned int NO_ENTITY_ID_BYTE = 0;

/**
 * @brief Player numbers just outside the players 1 to 4 of a game.
 */
constexpr unsigned int PLAYER_NUMBER_BYTE_BELOW_THE_FIRST = 0;
constexpr unsigned int PLAYER_NUMBER_BYTE_ABOVE_THE_LAST = 5;

/**
 * @returns A player with the given number, SOME_LIVES lives and SOME_SCORE
 * points.
 */
PlayerState somePlayer(std::uint8_t number);

/**
 * @returns The players numbered 1 to 4.
 */
std::vector<PlayerState> fourPlayers();

/**
 * @returns @p count Bydo, numbered from 1, at x equal to their index and
 * y = SOME_ENTITY_Y.
 */
std::vector<EntityState> someEntities(std::size_t count);

/**
 * @returns One player of number 1, invulnerable.
 */
PlayerState invulnerablePlayer();

/**
 * @returns The state of the protocol page: one player and one ship at
 * (EXAMPLE_X, EXAMPLE_Y), at SOME_TICK.
 */
WorldState oneShipState();

/**
 * @returns The payload of a state of one player and no entity, whose motion
 * byte is overwritten with @p motion, a byte the encoder never writes when
 * it holds a bit past PLAYER_MOTION_BITS.
 */
std::vector<std::byte> payloadWithMotion(std::uint8_t motion);

}  // namespace rtype::network::testing
