#include "EntityAnnouncements.hpp"
#include <cstdint>
#include <utility>
#include <vector>
#include "EntityDestroyed.hpp"
#include "EntityIdConstants.hpp"
#include "EntityReplicatorTestConstants.hpp"
#include "EntitySpawned.hpp"
#include "GameEvent.hpp"
#include "Position.hpp"
#include "ProtocolConstants.hpp"
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

network::PlayerState playerFlying(std::uint8_t playerNumber,
                                  std::uint32_t shipEntityId) {
  return network::PlayerState{.playerNumber = playerNumber,
                              .shipEntityId = shipEntityId,
                              .lives = FLYING_PLAYER_LIVES,
                              .score = 0,
                              .invulnerable = false};
}

network::PlayerState secondPilotFlying(bool invulnerable) {
  network::PlayerState player = playerFlying(SECOND_PLAYER_NUMBER, SHIP_ID);
  player.invulnerable = invulnerable;
  return player;
}

network::GameEvent damaged(std::uint16_t eventNumber, std::uint32_t entityId) {
  return network::GameEvent{.eventNumber = eventNumber,
                            .kind = network::GameEventKind::EntityDamaged,
                            .playerNumber = network::NO_PLAYER_NUMBER,
                            .entityId = entityId};
}

network::GameEvent playerHit(std::uint16_t eventNumber,
                             std::uint8_t playerNumber) {
  return network::GameEvent{.eventNumber = eventNumber,
                            .kind = network::GameEventKind::PlayerHit,
                            .playerNumber = playerNumber,
                            .entityId = identifiers::NO_ENTITY_ID};
}

}  // namespace rtype::client::testing
