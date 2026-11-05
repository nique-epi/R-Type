#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include <vector>
#include "MessageType.hpp"
#include "WorldState.hpp"

namespace rtype::network {

/**
 * @brief The state of the game at one server tick, written as the difference
 * from an earlier state the receiver acknowledged.
 *
 * `baseDistance` is how many ticks back that earlier state, its reference, was
 * taken; NO_REFERENCE_DISTANCE (0) means there is none, and `changed` then
 * lists every entity. Otherwise `changed` lists the entities that are new or
 * whose position, rounded to the wire, differs from the reference, and
 * `removed` the ids the reference lists and this state does not. A state too
 * big for one datagram is sent as `partCount` datagrams with the same tick
 * and the same reference, `partIndex` counting from 0; each holds every player
 * and a part of the two lists.
 */
struct DeltaWorldState {
  static constexpr MessageType TYPE = MessageType::DeltaWorldState;

  std::uint32_t tick;
  std::uint8_t baseDistance;
  std::uint8_t partIndex;
  std::uint8_t partCount;
  std::vector<PlayerState> players;
  std::vector<EntityState> changed;
  std::vector<std::uint32_t> removed;

  bool operator==(const DeltaWorldState&) const = default;
};

/**
 * @brief Reads one DeltaWorldState payload, as DeltaWorldStateCodec::decode()
 * does. A state too big for one datagram is written by
 * DeltaWorldStateCodec::encode(), which gives several payloads, so there is no
 * encodePayload() for it.
 * @throws What DeltaWorldStateCodec::decode() throws.
 */
[[nodiscard]] DeltaWorldState decodePayload(
    std::span<const std::byte> payload,
    std::type_identity<DeltaWorldState> type);

}  // namespace rtype::network
