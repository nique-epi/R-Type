#pragma once

#include <cstddef>

namespace rtype::network::testing {

/**
 * @brief The sizes the protocol page gives a WorldState payload: a fixed part
 * (tick and the two counts), 16 bytes per player, 9 per entity (an id, a type
 * and two i16 coordinates).
 */
constexpr std::size_t PACKED_STATE_FIXED_BYTES = 6;
constexpr std::size_t PACKED_PLAYER_BYTES = 16;
constexpr std::size_t PACKED_ENTITY_BYTES = 9;

/**
 * @brief The same payload without packing: the six bits of a motion take six
 * bytes instead of one (16 - 1 + 6), and the two coordinates two binary32
 * floats instead of two i16 (9 - 4 + 8).
 */
constexpr std::size_t UNPACKED_PLAYER_BYTES = 21;
constexpr std::size_t UNPACKED_ENTITY_BYTES = 13;

/**
 * @brief The players and entities of LARGE_WORLD_STATE_BODY, a state of a real
 * game of four: its payload of 268 bytes is 6 + 4 * 16 + 22 * 9.
 */
constexpr std::size_t CAPTURED_PLAYERS = 4;
constexpr std::size_t CAPTURED_ENTITIES = 22;

/**
 * @brief The inputs a PlayerInput repeats on a client whose last inputs the
 * server has not confirmed yet: the load-test bots measured about 5.5 on a
 * perfect network.
 */
constexpr std::size_t TYPICAL_REPEATED_INPUTS = 5;

/**
 * @brief The inputs the measured PlayerInput holds: its own and the ones it
 * repeats.
 */
constexpr std::size_t INPUTS_IN_A_TYPICAL_MESSAGE = TYPICAL_REPEATED_INPUTS + 1;

/**
 * @brief The sizes of a PlayerInput payload: an input number and a count, then
 * one byte of actions per input packed, PLAYER_ACTION_COUNT bytes per input
 * unpacked.
 */
constexpr std::size_t INPUT_FIXED_BYTES = 5;
constexpr std::size_t PACKED_ACTIONS_BYTES = 1;
constexpr std::size_t UNPACKED_ACTIONS_BYTES = 5;

}  // namespace rtype::network::testing
