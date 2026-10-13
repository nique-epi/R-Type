#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include "SerializationConstants.hpp"

namespace rtype::network {

constexpr std::size_t INT16_WIRE_SIZE = sizeof(std::int16_t);

/**
 * @brief Bytes of the packet header every datagram starts with, see the
 * Packet header section of the protocol page.
 */
constexpr std::size_t PACKET_HEADER_SIZE = 22;

/**
 * @brief Bytes before the payload of each message: its type (one byte) and its
 * payload size (two bytes).
 */
constexpr std::size_t MESSAGE_PREFIX_SIZE = UINT8_WIRE_SIZE + UINT16_WIRE_SIZE;

/**
 * @brief Players a game holds, so the most a WorldState lists.
 */
constexpr std::size_t MAXIMUM_PLAYERS = 4;

/**
 * @brief Bytes of a player in a WorldState: playerNumber, shipEntityId, lives,
 * score and invulnerable.
 */
constexpr std::size_t PLAYER_STATE_WIRE_SIZE =
    UINT8_WIRE_SIZE + UINT32_WIRE_SIZE + UINT8_WIRE_SIZE + UINT32_WIRE_SIZE +
    UINT8_WIRE_SIZE;

/**
 * @brief Bytes of an entity in a WorldState: entityId, entityType, x and y.
 */
constexpr std::size_t ENTITY_STATE_WIRE_SIZE =
    UINT32_WIRE_SIZE + UINT8_WIRE_SIZE + INT16_WIRE_SIZE + INT16_WIRE_SIZE;

/**
 * @brief Bytes of a WorldState that do not depend on its players and entities:
 * tick, playerCount and entityCount.
 */
constexpr std::size_t WORLD_STATE_FIXED_SIZE =
    UINT32_WIRE_SIZE + UINT8_WIRE_SIZE + UINT8_WIRE_SIZE;

/**
 * @brief The bytes a bool is sent as; any other byte is invalid.
 */
constexpr std::uint8_t WIRE_FALSE = 0;
constexpr std::uint8_t WIRE_TRUE = 1;

/**
 * @brief Wire steps in one logical unit: a coordinate is sent in eighths of a
 * unit.
 */
constexpr float WIRE_COORDINATE_STEPS_PER_UNIT = 8.0F;

/**
 * @brief The smallest and the largest count of eighths of a unit a wire
 * coordinate holds, that is -4096 and 4095.875 units.
 */
constexpr float MINIMUM_WIRE_STEPS =
    static_cast<float>(std::numeric_limits<std::int16_t>::min());
constexpr float MAXIMUM_WIRE_STEPS =
    static_cast<float>(std::numeric_limits<std::int16_t>::max());

}  // namespace rtype::network
