#include "EntityReplicatorFixture.hpp"
#include <cstddef>
#include "ComponentRegistry.hpp"
#include "EntityAnnouncements.hpp"
#include "EntityReplicator.hpp"
#include "EntityReplicatorTestConstants.hpp"
#include "RecordingSoundPlayer.hpp"
#include "WorldState.hpp"

namespace rtype::client::testing {

EntityReplicator& EntityReplicatorFixture::replicator() { return replicator_; }

engine::ComponentRegistry& EntityReplicatorFixture::components() {
  return components_;
}

void EntityReplicatorFixture::announceOneOfEachType() {
  replicator_.apply(
      spawned(SHIP_ID, network::WireEntityType::PlayerShip, SHIP_POSITION));
  replicator_.apply(spawned(PLAYER_MISSILE_ID,
                            network::WireEntityType::PlayerMissile,
                            PLAYER_MISSILE_POSITION));
  replicator_.apply(
      spawned(BYDO_ID, network::WireEntityType::Bydo, BYDO_POSITION));
  replicator_.apply(spawned(ENEMY_MISSILE_ID,
                            network::WireEntityType::EnemyMissile,
                            ENEMY_MISSILE_POSITION));
}

std::size_t EntityReplicatorFixture::entityCount() const {
  return entities_.size();
}

const RecordingSoundPlayer& EntityReplicatorFixture::sounds() const {
  return sounds_;
}

}  // namespace rtype::client::testing
