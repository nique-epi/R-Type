#pragma once

#include <gtest/gtest.h>
#include "PlayerSettings.hpp"
#include "RecordingSettingsStorage.hpp"
#include "SettingsService.hpp"
#include "SoundBank.hpp"
#include "SoundPlayer.hpp"

/** @brief A sound player over an empty sound bank, built and subscribed while
 * the settings of its service are those of a first launch. */
class SoundPlayerFixture : public ::testing::Test {
 protected:
  SoundPlayerFixture();

  rtype::client::SettingsService& service();
  rtype::client::SoundPlayer& player();

 private:
  rtype::client::SoundBank bank_;
  rtype::client::PlayerSettings settings_;
  RecordingSettingsStorage storage_;
  rtype::client::SettingsService service_;
  rtype::client::SoundPlayer player_;
};
