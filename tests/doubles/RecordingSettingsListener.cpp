#include "RecordingSettingsListener.hpp"
#include <vector>
#include "SettingId.hpp"

using rtype::client::SettingId;

void RecordingSettingsListener::settingChanged(SettingId changed) {
  changes_.push_back(changed);
}

const std::vector<SettingId>& RecordingSettingsListener::changes() const {
  return changes_;
}

void RecordingSettingsListener::forgetChanges() { changes_.clear(); }
