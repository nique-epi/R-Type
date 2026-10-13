#pragma once

#include <cstddef>
#include <span>
#include <vector>
#include "WorldState.hpp"

namespace rtype::network {

/**
 * @brief Turns a WorldState into the payloads of the WorldState messages, and
 * back.
 *
 * Integers are big-endian; a coordinate is a signed 16-bit count of eighths of
 * a unit, rounded to the nearest eighth and clamped to -4096 to 4095.875 units.
 */
class WorldStateCodec {
 public:
  /**
   * @brief Encodes @p state as one payload, or as several payloads with the
   * same tick when its entities do not fit one datagram. Each payload holds
   * every player and a part of the entities, in their order; a state without
   * entities gives one payload.
   * @throws TooManyPlayersException when the state lists more than
   * MAXIMUM_PLAYERS players.
   */
  [[nodiscard]] static std::vector<std::vector<std::byte>> encode(
      const WorldState& state);

  /**
   * @brief Decodes the payload of one WorldState message.
   * @throws BufferUnderflowException when the payload is shorter than it
   * announces.
   * @throws MalformedMessageException when it announces more than
   * MAXIMUM_PLAYERS players, holds an unknown entity type, or has bytes left
   * after the last entity.
   */
  [[nodiscard]] static WorldState decode(std::span<const std::byte> payload);

  /**
   * @returns How many entities one payload holds, next to @p playerCount
   * players, so that the datagram stays within MAX_DATAGRAM_SIZE once the
   * packet header and the message prefix are added. 125 for 4 players.
   */
  [[nodiscard]] static std::size_t maximumEntitiesPerMessage(
      std::size_t playerCount);
};

}  // namespace rtype::network
