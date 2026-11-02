#include <gtest/gtest.h>
#include "AudioTestConstants.hpp"
#include "AudioVolume.hpp"
#include "PlayerSettings.hpp"
#include "SettingId.hpp"

using rtype::client::changesEffectsVolume;
using rtype::client::changesMusicVolume;
using rtype::client::effectsVolumeOf;
using rtype::client::musicVolumeOf;
using rtype::client::PlayerSettings;
using rtype::client::SettingId;
using rtype::client::testing::A_MUSIC_VOLUME;
using rtype::client::testing::AN_EFFECTS_VOLUME;

/**
 * Given a player whose MUSIC volume is 35 %
 * When the volume of the music is read
 * Then the music plays at 35 of 100
 */
TEST(AudioVolume, TheMusicPlaysAtTheMusicVolumeOfThePlayer) {
  PlayerSettings settings;
  settings.musicVolume = A_MUSIC_VOLUME;

  EXPECT_FLOAT_EQ(musicVolumeOf(settings), 35.0F);
}

/**
 * Given a player whose EFFECTS volume is 55 %
 * When the volume of a sound is read
 * Then the sound plays at 55 of 100
 */
TEST(AudioVolume, ASoundPlaysAtTheEffectsVolumeOfThePlayer) {
  PlayerSettings settings;
  settings.effectsVolume = AN_EFFECTS_VOLUME;

  EXPECT_FLOAT_EQ(effectsVolumeOf(settings), 55.0F);
}

/**
 * Given a player who turned MUTE ALL on
 * When the volume of the music is read
 * Then the music is silent
 */
TEST(AudioVolume, MuteAllSilencesTheMusic) {
  PlayerSettings settings;
  settings.muted = true;

  EXPECT_FLOAT_EQ(musicVolumeOf(settings), 0.0F);
}

/**
 * Given a player who turned MUTE ALL on
 * When the volume of a sound is read
 * Then the sound is silent
 */
TEST(AudioVolume, MuteAllSilencesTheSounds) {
  PlayerSettings settings;
  settings.muted = true;

  EXPECT_FLOAT_EQ(effectsVolumeOf(settings), 0.0F);
}

/**
 * Given the settings a listener can be told about
 * When the music asks which of them move its volume
 * Then only the music volume and MUTE ALL do
 */
TEST(AudioVolume, OnlyTheMusicVolumeAndMuteAllMoveTheMusic) {
  EXPECT_TRUE(changesMusicVolume(SettingId::MusicVolume));
  EXPECT_TRUE(changesMusicVolume(SettingId::Muted));
  EXPECT_FALSE(changesMusicVolume(SettingId::EffectsVolume));
  EXPECT_FALSE(changesMusicVolume(SettingId::AutoFire));
}

/**
 * Given the settings a listener can be told about
 * When the sounds ask which of them move their volume
 * Then only the effects volume and MUTE ALL do
 */
TEST(AudioVolume, OnlyTheEffectsVolumeAndMuteAllMoveTheSounds) {
  EXPECT_TRUE(changesEffectsVolume(SettingId::EffectsVolume));
  EXPECT_TRUE(changesEffectsVolume(SettingId::Muted));
  EXPECT_FALSE(changesEffectsVolume(SettingId::MusicVolume));
  EXPECT_FALSE(changesEffectsVolume(SettingId::AutoFire));
}
