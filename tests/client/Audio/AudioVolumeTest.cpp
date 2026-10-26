#include <gtest/gtest.h>
#include "AudioTestConstants.hpp"
#include "AudioVolume.hpp"
#include "PlayerSettings.hpp"

using rtype::client::effectsVolumeOf;
using rtype::client::musicVolumeOf;
using rtype::client::PlayerSettings;
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
