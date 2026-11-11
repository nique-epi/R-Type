#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>
#include "PlayerInput.hpp"
#include "WorldState.hpp"

namespace rtype::network::testing {

/**
 * @returns The world state of a body the real server sent in protocol version
 * 15: @p body without its message prefix, a look 0 written after each entity
 * as version 16 carries it, decoded by WorldStateCodec.
 */
WorldState capturedWorldState(std::span<const std::uint8_t> body);

/**
 * @returns @p state written as the protocol would without its packing, field by
 * field in the order WorldStateCodec writes them: each coordinate an IEEE-754
 * binary32 float instead of an i16 of eighths, each of the
 * PLAYER_MOTION_BIT_COUNT bits of a motion a byte of its own instead of one
 * byte for all.
 */
std::vector<std::byte> unpackedWorldStateBytes(const WorldState& state);

/**
 * @returns The payload of @p input written as the protocol would without its
 * packing: each of the PLAYER_ACTION_COUNT actions of each input a byte of its
 * own instead of one bit.
 */
std::vector<std::byte> unpackedPlayerInputBytes(const PlayerInput& input);

}  // namespace rtype::network::testing
