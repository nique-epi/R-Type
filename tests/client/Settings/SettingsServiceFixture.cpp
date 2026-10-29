#include "SettingsServiceFixture.hpp"

SettingsServiceFixture::SettingsServiceFixture()
    : service_(settings_, storage_) {
  service_.subscribe(listener_);
}
