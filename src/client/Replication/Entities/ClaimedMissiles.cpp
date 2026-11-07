#include "ClaimedMissiles.hpp"
#include <algorithm>
#include <cstdint>
#include "EntityDestroyed.hpp"
#include "EntitySpawned.hpp"
#include "IOwnMissileListener.hpp"
#include "ProtocolConstants.hpp"
#include "ReplicationConstants.hpp"

namespace rtype::client {

ClaimedMissiles::ClaimedMissiles(std::uint8_t ownPlayerNumber,
                                 IOwnMissileListener* listener)
    : ownPlayerNumber_(ownPlayerNumber), listener_(listener) {}

bool ClaimedMissiles::claim(const network::EntitySpawned& spawned) {
  if (listener_ == nullptr || !listener_->predictsOwnMissiles() ||
      spawned.owner == network::NO_OWNER || spawned.owner != ownPlayerNumber_) {
    return false;
  }
  if (!holds(spawned.entityId)) {
    if (entityIds_.size() == CLAIMED_MISSILE_COUNT) {
      entityIds_.erase(entityIds_.begin());
    }
    entityIds_.push_back(spawned.entityId);
    listener_->ownMissileSpawned(spawned);
  }
  return true;
}

bool ClaimedMissiles::holds(std::uint32_t entityId) const {
  return std::ranges::find(entityIds_, entityId) != entityIds_.end();
}

bool ClaimedMissiles::release(const network::EntityDestroyed& destroyed) {
  const auto claimed = std::ranges::find(entityIds_, destroyed.entityId);
  if (claimed == entityIds_.end()) {
    return false;
  }
  entityIds_.erase(claimed);
  if (listener_ != nullptr) {
    listener_->ownMissileDestroyed(destroyed);
  }
  return true;
}

}  // namespace rtype::client
