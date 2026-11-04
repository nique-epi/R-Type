#pragma once

#include <cstddef>
#include <span>
#include <vector>
#include "DeltaWorldState.hpp"

namespace rtype::network {

/**
 * @brief Turns a DeltaWorldState into the payloads of the DeltaWorldState
 * messages, and back. It writes integers and coordinates as WorldStateCodec
 * does.
 */
class DeltaWorldStateCodec {
 public:
  /**
   * @brief Encodes @p whole as one payload, or as several with the same tick
   * and reference when its lists do not fit one datagram. Each payload holds
   * every player and a part of `changed`, then of `removed`, in their order;
   * `partIndex` and `partCount` of @p whole are not read, each payload gets
   * its own. A state without entities gives one payload.
   * @throws TooManyPlayersException when the state lists more than
   * MAXIMUM_PLAYERS players.
   * @throws TooManyDeltaPartsException when it needs more than
   * MAXIMUM_DELTA_PARTS payloads.
   * @throws InvalidCoordinateException when an entity coordinate is NaN.
   */
  [[nodiscard]] static std::vector<std::vector<std::byte>> encode(
      const DeltaWorldState& whole);

  /**
   * @brief Decodes the payload of one DeltaWorldState message: a part, with
   * its own partIndex and partCount.
   * @throws BufferUnderflowException when the payload is shorter than it
   * announces.
   * @throws MalformedMessageException when baseDistance is above
   * MAXIMUM_REFERENCE_DISTANCE, partCount is 0 or above MAXIMUM_DELTA_PARTS,
   * partIndex is not below partCount, an entity or a removed id is 0, a
   * player is invalid (see WorldStateCodec::decode()), or bytes follow the
   * last removed id. An entity of unknown type is left out of `changed`.
   */
  [[nodiscard]] static DeltaWorldState decode(
      std::span<const std::byte> payload);
};

}  // namespace rtype::network
