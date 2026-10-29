#include "SettingsServiceFixture.hpp"
#include "PlayerSettings.hpp"
#include "RecordingSettingsListener.hpp"
#include "RecordingSettingsStorage.hpp"
#include "SettingsService.hpp"

SettingsServiceFixture::SettingsServiceFixture()
    : service_(settings_, storage_) {
  service_.subscribe(listener_);
}

rtype::client::PlayerSettings& SettingsServiceFixture::settings() {
  return settings_;
}

RecordingSettingsStorage& SettingsServiceFixture::storage() { return storage_; }

RecordingSettingsListener& SettingsServiceFixture::listener() {
  return listener_;
}

rtype::client::SettingsService& SettingsServiceFixture::service() {
  return service_;
}
