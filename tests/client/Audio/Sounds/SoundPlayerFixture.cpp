#include "SoundPlayerFixture.hpp"
#include "SettingsService.hpp"
#include "SoundPlayer.hpp"

SoundPlayerFixture::SoundPlayerFixture()
    : service_(settings_, storage_), player_(bank_, service_) {
  service_.subscribe(player_);
}

rtype::client::SettingsService& SoundPlayerFixture::service() {
  return service_;
}

rtype::client::SoundPlayer& SoundPlayerFixture::player() { return player_; }
