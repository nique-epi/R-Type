#include "EntityAnnouncements.hpp"
#include <cstdint>
#include <utility>
#include <vector>
#include "EntityDestroyed.hpp"
#include "EntitySpawned.hpp"
#include "Position.hpp"
#include "WorldState.hpp"

namespace rtype::client::testing {

network::EntitySpawned spawned(std::uint32_t entityId,
                               network::WireEntityType type,
                               game::Position position) {
  return network::EntitySpawned{.entityId = entityId,
                                .entityType = type,
                                .x = position.x,
                                .y = position.y};
}

network::EntityState listed(std::uint32_t entityId,
                            network::WireEntityType type,
                            game::Position position) {
  return network::EntityState{.entityId = entityId,
                              .entityType = type,
                              .x = position.x,
                              .y = position.y};
}

network::WorldState stateListing(std::uint32_t tick,
                                 std::vector<network::EntityState> entities) {
  return network::WorldState{
      .tick = tick, .players = {}, .entities = std::move(entities)};
}

network::WorldState stateListingPlayers(
    std::uint32_t tick, std::vector<network::PlayerState> players) {
  return network::WorldState{
      .tick = tick, .players = std::move(players), .entities = {}};
}

network::EntityDestroyed killed(std::uint32_t entityId) {
  return network::EntityDestroyed{.entityId = entityId,
                                  .reason = network::DestructionReason::Killed};
}

network::EntityDestroyed removed(std::uint32_t entityId) {
  return network::EntityDestroyed{
      .entityId = entityId, .reason = network::DestructionReason::Removed};
}

}  // namespace rtype::client::testing
