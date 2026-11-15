#include <gtest/gtest.h>
#include "AudioConstants.hpp"
#include "AudioTestConstants.hpp"
#include "AudioVolume.hpp"
#include "PlayerSettings.hpp"
#include "SettingId.hpp"

using rtype::client::BOSS_TRACK;
using rtype::client::changesEffectsVolume;
using rtype::client::changesMusicVolume;
using rtype::client::changesVoicesVolume;
using rtype::client::CROWD_AMBIENCE_TRACK;
using rtype::client::effectsVolumeOf;
using rtype::client::HANGAR_AMBIENCE_TRACK;
using rtype::client::musicVolumeOf;
using rtype::client::PlayerSettings;
using rtype::client::SettingId;
using rtype::client::SPACE_AMBIENCE_TRACK;
using rtype::client::THEME_TRACK;
using rtype::client::voicesVolumeOf;
using rtype::client::testing::A_MUSIC_VOLUME;
using rtype::client::testing::A_VOICES_VOLUME;
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
 * Given a player whose EFFECTS volume is 55 % and VOICES volume 25 %
 * When the volume of a voice is read
 * Then the voice plays at 25 of 100, not at the volume of the effects
 */
TEST(AudioVolume, AVoicePlaysAtTheVoicesVolumeOfThePlayer) {
  PlayerSettings settings;
  settings.effectsVolume = AN_EFFECTS_VOLUME;
  settings.voicesVolume = A_VOICES_VOLUME;

  EXPECT_FLOAT_EQ(voicesVolumeOf(settings), 25.0F);
}

/**
 * Given a player who turned MUTE ALL on
 * When the volume of a voice is read
 * Then the voice is silent
 */
TEST(AudioVolume, MuteAllSilencesTheVoices) {
  PlayerSettings settings;
  settings.muted = true;

  EXPECT_FLOAT_EQ(voicesVolumeOf(settings), 0.0F);
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
  EXPECT_FALSE(changesEffectsVolume(SettingId::VoicesVolume));
  EXPECT_FALSE(changesEffectsVolume(SettingId::AutoFire));
}

/**
 * Given the settings a listener can be told about
 * When the voices ask which of them move their volume
 * Then only the voices volume and MUTE ALL do
 */
TEST(AudioVolume, OnlyTheVoicesVolumeAndMuteAllMoveTheVoices) {
  EXPECT_TRUE(changesVoicesVolume(SettingId::VoicesVolume));
  EXPECT_TRUE(changesVoicesVolume(SettingId::Muted));
  EXPECT_FALSE(changesVoicesVolume(SettingId::EffectsVolume));
  EXPECT_FALSE(changesVoicesVolume(SettingId::MusicVolume));
}

/**
 * Given the musics that carry a screen and the ambiences that play under
 * the theme
 * When the gain of each against the MUSIC volume is read
 * Then the musics play at the full volume, the space at 0.25, the garage at
 * 0.40 and the crowd of the hangar at 0.50
 */
TEST(AudioVolume, AmbiencesPlayUnderTheMusicsThatCarryAScreen) {
  constexpr float FULL = 1.0F;
  constexpr float A_QUARTER = 0.25F;
  constexpr float FORTY_HUNDREDTHS = 0.40F;
  constexpr float HALF = 0.50F;
  EXPECT_FLOAT_EQ(THEME_TRACK.gain, FULL);
  EXPECT_FLOAT_EQ(BOSS_TRACK.gain, FULL);
  EXPECT_FLOAT_EQ(SPACE_AMBIENCE_TRACK.gain, A_QUARTER);
  EXPECT_FLOAT_EQ(HANGAR_AMBIENCE_TRACK.gain, FORTY_HUNDREDTHS);
  EXPECT_FLOAT_EQ(CROWD_AMBIENCE_TRACK.gain, HALF);
}
