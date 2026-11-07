#include "RecordingOwnMissileListener.hpp"
#include <vector>
#include "EntityDestroyed.hpp"
#include "EntitySpawned.hpp"

void RecordingOwnMissileListener::predict(bool predicts) {
  predicts_ = predicts;
}

bool RecordingOwnMissileListener::predictsOwnMissiles() const {
  return predicts_;
}

void RecordingOwnMissileListener::ownMissileSpawned(
    const rtype::network::EntitySpawned& missile) {
  spawned_.push_back(missile);
}

void RecordingOwnMissileListener::ownMissileDestroyed(
    const rtype::network::EntityDestroyed& destroyed) {
  destroyed_.push_back(destroyed);
}

const std::vector<rtype::network::EntitySpawned>&
RecordingOwnMissileListener::spawned() const {
  return spawned_;
}

const std::vector<rtype::network::EntityDestroyed>&
RecordingOwnMissileListener::destroyed() const {
  return destroyed_;
}
