#pragma once

#include <cstddef>
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
 * @returns A delta of four players and @p changedCount changed Bydo numbered
 * from 1, without a reference and with nothing removed.
 */
DeltaWorldState deltaWithChanged(std::size_t changedCount);

/**
 * @returns A delta of four players and @p removedCount removed ids numbered
 * from 1, without a reference and with nothing changed.
 */
DeltaWorldState deltaWithRemoved(std::size_t removedCount);

/**
 * @returns @p payload with the byte at @p offset replaced by @p value.
 */
std::vector<std::byte> withByteAt(std::vector<std::byte> payload,
                                  std::size_t offset, unsigned int value);

}  // namespace rtype::network::testing
