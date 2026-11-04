#include "WorldStateAssemblerFixtures.hpp"
#include <cstdint>
#include <utility>
#include <vector>
#include "DeltaWorldState.hpp"
#include "ProtocolConstants.hpp"
#include "StateDelta.hpp"
#include "WorldState.hpp"

namespace rtype::client::testing {

namespace {

network::PlayerState onlyPlayer() {
  return network::PlayerState{.playerNumber = network::FIRST_PLAYER_NUMBER,
                              .shipEntityId = 1,
                              .lives = 3,
                              .score = 0,
                              .invulnerable = false,
                              .motion = 0,
                              .acknowledgedInput = 0};
}

}  // namespace

network::EntityState bydoAt(std::uint32_t entityId, float x) {
  return network::EntityState{.entityId = entityId,
                              .entityType = network::WireEntityType::Bydo,
                              .x = x,
                              .y = 0.0F};
}

network::DeltaWorldState fullPart(std::uint32_t tick,
                                  std::vector<network::EntityState> entities) {
  return deltaPart(
      tick, network::NO_REFERENCE_DISTANCE,
      network::StateDelta{.changed = std::move(entities), .removed = {}});
}

network::DeltaWorldState deltaPart(std::uint32_t tick,
                                   std::uint8_t baseDistance,
                                   network::StateDelta changes) {
  return network::DeltaWorldState{.tick = tick,
                                  .baseDistance = baseDistance,
                                  .partIndex = 0,
                                  .partCount = 1,
                                  .players = {onlyPlayer()},
                                  .changed = std::move(changes.changed),
                                  .removed = std::move(changes.removed)};
}

network::DeltaWorldState partOf(network::DeltaWorldState whole,
                                PartPosition position) {
  whole.partIndex = position.index;
  whole.partCount = position.count;
  return whole;
}

}  // namespace rtype::client::testing
