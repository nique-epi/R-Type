#include "EntityReplicatorFixture.hpp"
#include <cstddef>
#include "ComponentRegistry.hpp"
#include "EntityAnnouncements.hpp"
#include "EntityReplicator.hpp"
#include "EntityReplicatorTestConstants.hpp"
#include "FixedEntityTypeSourceConstants.hpp"
#include "RecordingOwnMissileListener.hpp"
#include "RecordingSoundPlayer.hpp"

namespace rtype::client::testing {

EntityReplicator& EntityReplicatorFixture::replicator() { return replicator_; }

engine::ComponentRegistry& EntityReplicatorFixture::components() {
  return components_;
}

void EntityReplicatorFixture::announceOneOfEachType() {
  replicator_.apply(spawned(SHIP_ID, SHIP_TYPE_NUMBER, SHIP_POSITION));
  replicator_.apply(spawned(PLAYER_MISSILE_ID, PLAYER_MISSILE_TYPE_NUMBER,
                            PLAYER_MISSILE_POSITION));
  replicator_.apply(spawned(BYDO_ID, DRIFTER_TYPE_NUMBER, BYDO_POSITION));
  replicator_.apply(spawned(ENEMY_MISSILE_ID, ENEMY_MISSILE_TYPE_NUMBER,
                            ENEMY_MISSILE_POSITION));
}

std::size_t EntityReplicatorFixture::entityCount() const {
  return entities_.size();
}

const RecordingSoundPlayer& EntityReplicatorFixture::sounds() const {
  return sounds_;
}

RecordingOwnMissileListener& EntityReplicatorFixture::ownMissiles() {
  return ownMissiles_;
}

}  // namespace rtype::client::testing
