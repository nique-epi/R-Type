#pragma once

#include <gtest/gtest.h>
#include "AssetLibrary.hpp"
#include "PlayerSettings.hpp"
#include "RecordingSettingsStorage.hpp"
#include "SettingsService.hpp"
#include "SoundPlayer.hpp"
#include "TemporaryFolder.hpp"

/** @brief A sound player over an asset library on an empty folder, built and
 * subscribed while the settings of its service are those of a first launch. */
class SoundPlayerFixture : public ::testing::Test {
 protected:
  SoundPlayerFixture();

  rtype::client::SettingsService& service();
  rtype::client::SoundPlayer& player();

 private:
  TemporaryFolder folder_;
  rtype::client::AssetLibrary library_;
  rtype::client::PlayerSettings settings_;
  RecordingSettingsStorage storage_;
  rtype::client::SettingsService service_;
  rtype::client::SoundPlayer player_;
};
