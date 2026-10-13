#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include "NetworkConstants.hpp"
#include "ProtocolConstants.hpp"
#include "WorldState.hpp"

namespace rtype::network {

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

}  // namespace rtype::network
