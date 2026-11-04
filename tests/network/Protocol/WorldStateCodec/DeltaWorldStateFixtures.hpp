#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include "DeltaWorldState.hpp"

namespace rtype::network::testing {

/**
 * @returns The state of the protocol page as a delta of one part, whose
 * reference lies EXAMPLE_BASE_DISTANCE ticks back and which removes the entity
 * EXAMPLE_REMOVED_ENTITY_ID.
 */
DeltaWorldState oneShipDelta();

/**
 * @returns A delta of four players, @p changedCount changed Bydo numbered from
 * 1 and @p removedCount removed ids numbered from 1, without a reference.
 */
DeltaWorldState deltaOfLists(std::size_t changedCount,
                             std::size_t removedCount);

/**
 * @returns @p payload with the byte at @p offset replaced by @p value.
 */
std::vector<std::byte> withByteAt(std::vector<std::byte> payload,
                                  std::size_t offset, unsigned int value);

}  // namespace rtype::network::testing
