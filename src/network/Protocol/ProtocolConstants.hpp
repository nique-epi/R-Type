#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include "NetworkConstants.hpp"
#include "SerializationConstants.hpp"
#include "WorldState.hpp"

namespace rtype::network {

constexpr std::size_t INT16_WIRE_SIZE = sizeof(std::int16_t);

/**
 * @brief The two values a boolean takes on the wire.
 */
constexpr std::uint8_t FALSE_WIRE_VALUE = 0;
constexpr std::uint8_t TRUE_WIRE_VALUE = 1;

/**
 * @brief The entity id no entity has.
 */
constexpr std::uint32_t NO_ENTITY_ID = 0;

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
 * @brief Wire steps in one logical unit: a coordinate is sent in eighths of a
 * unit.
 */
constexpr float WIRE_COORDINATE_STEPS_PER_UNIT = 8.0F;

/**
 * @brief Bytes a WorldState payload may hold: a datagram, less the packet
 * header and the message prefix that go around it.
 */
constexpr std::size_t WORLD_STATE_PAYLOAD_LIMIT =
    MAX_DATAGRAM_SIZE - PACKET_HEADER_SIZE - MESSAGE_PREFIX_SIZE;

static_assert((WORLD_STATE_PAYLOAD_LIMIT - WORLD_STATE_FIXED_SIZE) /
                      ENTITY_STATE_WIRE_SIZE <=
                  std::numeric_limits<std::uint8_t>::max(),
              "entityCount is one byte: it must hold what a datagram fits");

/**
 * @brief The smallest and the largest value of an entity type on the wire.
 */
constexpr std::uint8_t FIRST_WIRE_ENTITY_TYPE =
    static_cast<std::uint8_t>(WireEntityType::PlayerShip);
constexpr std::uint8_t LAST_WIRE_ENTITY_TYPE =
    static_cast<std::uint8_t>(WireEntityType::EnemyMissile);

/**
 * @brief The smallest and the largest count of eighths of a unit a wire
 * coordinate holds, that is -4096 and 4095.875 units.
 */
constexpr float MINIMUM_WIRE_STEPS =
    static_cast<float>(std::numeric_limits<std::int16_t>::min());
constexpr float MAXIMUM_WIRE_STEPS =
    static_cast<float>(std::numeric_limits<std::int16_t>::max());

}  // namespace rtype::network
