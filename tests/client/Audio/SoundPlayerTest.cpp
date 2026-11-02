#include <gtest/gtest.h>
#include "AudioTestConstants.hpp"
#include "ClientException.hpp"
#include "PlayerSettings.hpp"
#include "SoundPlayerFixture.hpp"

using rtype::client::AssetNotLoadedException;
using rtype::client::PlayerSettings;
using rtype::client::testing::UNKNOWN_SOUND_ID;

/**
 * Given a sound player over the settings of a first launch, which can be heard
 * When it plays a sound no asset has
 * Then it looks for the sound and fails
 */
TEST_F(SoundPlayerFixture, APlayerThatCanBeHeardLooksForItsSound) {
  EXPECT_THROW(player().play(UNKNOWN_SOUND_ID), AssetNotLoadedException);
}

/**
 * Given a sound player built before the settings file was read
 * When the settings read, with MUTE ALL on, are restored
 * Then it plays nothing, and never looks for a sound no asset has
 */
TEST_F(SoundPlayerFixture, APlayerBuiltBeforeTheSettingsWereReadFollowsThem) {
  PlayerSettings kept;
  kept.muted = true;

  service().restore(kept);

  EXPECT_NO_THROW(player().play(UNKNOWN_SOUND_ID));
}
