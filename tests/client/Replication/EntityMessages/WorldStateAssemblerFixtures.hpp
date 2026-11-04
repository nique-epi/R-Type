#pragma once

#include <cstdint>
#include <vector>
#include "DeltaWorldState.hpp"
#include "StateDelta.hpp"
#include "WorldState.hpp"

namespace rtype::client::testing {

/** @brief Where a part stands among the parts of its state. */
struct PartPosition {
  std::uint8_t index;
  std::uint8_t count;
};

/**
 * @returns A Bydo of id @p entityId at (@p x, 0).
 */
network::EntityState bydoAt(std::uint32_t entityId, float x);

/**
 * @returns A state of one part at @p tick with no reference, listing
 * @p entities and one player.
 */
network::DeltaWorldState fullPart(std::uint32_t tick,
                                  std::vector<network::EntityState> entities);

/**
 * @returns A state of one part at @p tick written against the state
 * @p baseDistance ticks back, with @p changes, and one player.
 */
network::DeltaWorldState deltaPart(std::uint32_t tick,
                                   std::uint8_t baseDistance,
                                   network::StateDelta changes);

/**
 * @returns @p whole as the part at @p position of its state.
 */
network::DeltaWorldState partOf(network::DeltaWorldState whole,
                                PartPosition position);

}  // namespace rtype::client::testing
