#include "DeltaWorldStateFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include "DeltaWorldState.hpp"
#include "DeltaWorldStateCodecTestConstants.hpp"
#include "ProtocolConstants.hpp"
#include "WorldState.hpp"
#include "WorldStateFixtures.hpp"

namespace rtype::network::testing {

DeltaWorldState oneShipDelta() {
  const WorldState state = oneShipState();
  return DeltaWorldState{.tick = state.tick,
                         .baseDistance = EXAMPLE_BASE_DISTANCE,
                         .partIndex = 0,
                         .partCount = 1,
                         .players = state.players,
                         .changed = state.entities,
                         .removed = {EXAMPLE_REMOVED_ENTITY_ID}};
}

DeltaWorldState deltaOfLists(std::size_t changedCount,
                             std::size_t removedCount) {
  std::vector<std::uint32_t> removed;
  removed.reserve(removedCount);
  for (std::size_t index = 0; index < removedCount; ++index) {
    removed.push_back(static_cast<std::uint32_t>(index + 1));
  }
  return DeltaWorldState{.tick = SOME_TICK,
                         .baseDistance = NO_REFERENCE_DISTANCE,
                         .partIndex = 0,
                         .partCount = 1,
                         .players = fourPlayers(),
                         .changed = someEntities(changedCount),
                         .removed = std::move(removed)};
}

std::vector<std::byte> withByteAt(std::vector<std::byte> payload,
                                  std::size_t offset, unsigned int value) {
  payload.at(offset) = static_cast<std::byte>(value);
  return payload;
}

}  // namespace rtype::network::testing
