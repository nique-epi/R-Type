#pragma once

#include <cstddef>
#include <cstdint>
#include "Position.hpp"

namespace rtype::server::testing {

/** @brief The network ids of the two enemies the tests place, the first of
 * which is the one that moves or vanishes. */
constexpr std::uint32_t MOVING_ENEMY_ID = 1;
constexpr std::uint32_t STILL_ENEMY_ID = 2;

/** @brief Where the enemies start, and where the first one moves to. */
constexpr game::Position FIRST_PLACE{.x = 10.0F, .y = 20.0F};
constexpr game::Position SECOND_PLACE{.x = 30.0F, .y = 40.0F};
constexpr game::Position MOVED_PLACE{.x = 11.0F, .y = 21.0F};

/** @brief Ticks of one world state at the default rate: 60 / 20. */
constexpr std::size_t TICKS_PER_STATE = 3;

/** @brief The ticks run until the first state, and until the second. */
constexpr std::size_t TICKS_TO_THE_FIRST_STATE = 3;

/** @brief Ticks after the first state, at which the state still reaches back
 * to it as far as a delta may (60), and the state after that one (63). */
constexpr std::size_t TICKS_TO_THE_FARTHEST_REFERENCE = 60;
constexpr std::size_t TICKS_BEYOND_THE_FARTHEST_REFERENCE = 63;

/** @brief Enemies that need three datagrams to list in full. */
constexpr std::size_t ENEMIES_FOR_THREE_PARTS = 300;

/** @brief The ports of the two clients. */
constexpr std::uint16_t FIRST_PLAYER_PORT = 4100;
constexpr std::uint16_t SECOND_PLAYER_PORT = 4101;

/** @brief The scene of the measure: ten seconds, so 600 ticks and 200 states
 * at the default rate, of 30 enemies. */
constexpr std::size_t MEASURED_ENTITIES = 30;
constexpr std::size_t MEASURED_TICKS = 600;
constexpr std::size_t MEASURED_STATES = 200;
constexpr std::size_t MEASURED_SECONDS = 10;

/** @brief The enemies that move in the measure of half the scene. */
constexpr std::size_t HALF_THE_ENTITIES = 15;

/** @brief The bytes of a state of one datagram, from the protocol page: the
 * packet header and the message prefix (22 + 3), the fixed part of a
 * DeltaWorldState (10), one player (16), then 9 per entity listed. */
constexpr std::size_t HEADER_AND_PREFIX_BYTES = 25;
constexpr std::size_t DELTA_FIXED_BYTES = 10;
constexpr std::size_t PLAYER_BYTES = 16;
constexpr std::size_t ENTITY_BYTES = 9;
constexpr std::size_t STATE_BYTES_WITHOUT_ENTITIES =
    HEADER_AND_PREFIX_BYTES + DELTA_FIXED_BYTES + PLAYER_BYTES;
constexpr std::size_t FULL_STATE_BYTES =
    STATE_BYTES_WITHOUT_ENTITIES + (MEASURED_ENTITIES * ENTITY_BYTES);

/** @brief The bits of acknowledgementBits of a client that accepted every
 * datagram before the newest, and of one that accepted none of them. */
constexpr std::uint32_t EVERY_EARLIER_DATAGRAM = 0xFFFFFFFFU;
constexpr std::uint32_t ONLY_THE_NEWEST_DATAGRAM = 0;

}  // namespace rtype::server::testing
