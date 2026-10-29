#pragma once

#include <gtest/gtest.h>
#include "PlayerSettings.hpp"
#include "RecordingSettingsListener.hpp"
#include "RecordingSettingsStorage.hpp"
#include "SettingsService.hpp"

/** @brief A service over settings of a first launch, a storage that records
 * its saves and one subscribed listener. */
class SettingsServiceFixture : public ::testing::Test {
 protected:
  SettingsServiceFixture();

  rtype::client::PlayerSettings settings_;
  RecordingSettingsStorage storage_;
  RecordingSettingsListener listener_;
  rtype::client::SettingsService service_;
};
