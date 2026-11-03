#include <gtest/gtest.h>
#include "AudioTestConstants.hpp"
#include "ScreenTestConstants.hpp"
#include "SilentAudioService.hpp"

using rtype::client::SilentAudioService;
using rtype::client::testing::UNKNOWN_SOUND_ID;

/**
 * Given the audio service of a client without sound
 * When a screen plays a sound no asset has
 * Then the call is accepted
 */
TEST(SilentAudioService, SoundsAcceptAnyId) {
  SilentAudioService audio;

  EXPECT_NO_THROW(audio.sounds().play(UNKNOWN_SOUND_ID));
}

/**
 * Given the audio service of a client without sound
 * When the intro starts the theme
 * Then the theme still reports no position
 */
TEST(SilentAudioService, TheThemeNeverReportsAPosition) {
  SilentAudioService audio;

  audio.theme().start();

  EXPECT_FALSE(audio.theme().playingOffset().has_value());
}

/**
 * Given the audio service of a client without sound
 * When the screens ask for the theme, the music of a game, the opening of the
 * files and an update
 * Then every call is accepted
 */
TEST(SilentAudioService, EveryCallOfTheScreensAndTheModuleIsAccepted) {
  SilentAudioService audio;

  EXPECT_NO_THROW({
    audio.playTheme();
    audio.playGameMusic();
    audio.open();
    audio.update(ONE_FRAME);
  });
}
