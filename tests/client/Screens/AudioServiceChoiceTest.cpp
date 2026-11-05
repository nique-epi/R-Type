#include <gtest/gtest.h>
#include "AudioServiceChoiceFixture.hpp"
#include "IAudioService.hpp"

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
 * Given an audio service provided by a module installed before the screens
 * When the screens choose their audio service
 * Then they get that service, not their own silence
 */
TEST_F(AudioServiceChoiceFixture, TheScreensGetTheServiceProvidedBeforeThem) {
  provideAnAudioService();

  const IAudioService& chosen = chosenByTheScreens();

  EXPECT_EQ(&chosen, &provided());
}
