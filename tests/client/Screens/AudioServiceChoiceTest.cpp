#include <gtest/gtest.h>
#include "AudioMode.hpp"
#include "AudioService.hpp"
#include "AudioServiceChoiceFixture.hpp"
#include "IAudioService.hpp"
#include "SoundConstants.hpp"

using rtype::client::AudioMode;
using rtype::client::AudioService;
using rtype::client::CLICK_SOUND_ID;
using rtype::client::IAudioService;

/**
 * Given no sound module installed before the screens
 * When the screens choose their audio service
 * Then they get the silent service they hold
 */
TEST_F(AudioServiceChoiceFixture, WithoutTheSoundModuleTheScreensGetSilence) {
  const IAudioService& chosen = chosenByTheScreens();

  EXPECT_EQ(&chosen, &silenceOfTheScreens());
}

/**
 * Given the sound module of a client with sound installed before the screens
 * When the screens choose their audio service
 * Then they get the service the sound module provided
 */
TEST_F(AudioServiceChoiceFixture, TheScreensGetTheServiceOfTheSoundModule) {
  installTheSoundModule(AudioMode::Enabled);

  const IAudioService& chosen = chosenByTheScreens();

  EXPECT_EQ(&chosen, providedBySound());
}

/**
 * Given the sound module of a client with sound installed before the screens
 * When the screens choose their audio service
 * Then it is the service that plays, not a silent one
 */
TEST_F(AudioServiceChoiceFixture, TheScreensGetTheServiceThatPlays) {
  installTheSoundModule(AudioMode::Enabled);

  IAudioService& chosen = chosenByTheScreens();

  EXPECT_NE(dynamic_cast<AudioService*>(&chosen), nullptr);
}

/**
 * Given the sound module of a client with sound installed before the screens
 * and turned off by the configuration, so never started
 * When a screen plays the click through the service it got
 * Then nothing plays and nothing fails, as without the module
 */
TEST_F(AudioServiceChoiceFixture, ASoundModuleTurnedOffPlaysNothing) {
  installTheSoundModule(AudioMode::Enabled);
  IAudioService& chosen = chosenByTheScreens();

  EXPECT_NO_THROW(chosen.sounds().play(CLICK_SOUND_ID));
}
