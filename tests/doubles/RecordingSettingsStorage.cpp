#include "RecordingSettingsStorage.hpp"
#include <vector>
#include "PlayerSettings.hpp"

bool RecordingSettingsStorage::save(
    const rtype::client::PlayerSettings& settings) {
  saves_.push_back(settings);
  return !refused_;
}

void RecordingSettingsStorage::refuseSaves(bool refused) { refused_ = refused; }

const std::vector<rtype::client::PlayerSettings>&
RecordingSettingsStorage::saves() const {
  return saves_;
}
