#include <gtest/gtest.h>
#include "AudioTestConstants.hpp"
#include "FakeMusicPlayer.hpp"
#include "MusicFade.hpp"

using rtype::client::MusicFade;
using rtype::client::testing::HALF_THE_FADE;
using rtype::client::testing::HALF_THE_VOLUME;
using rtype::client::testing::SILENT;
using rtype::client::testing::THE_FULL_VOLUME;
using rtype::client::testing::THE_WHOLE_FADE;

/**
 * Given a music that does not play
 * When a fade of 3 s begins on it
 * Then it plays, silent
 */
TEST(MusicFade, AFadeStartsTheMusicSilent) {
  FakeMusicPlayer music;
  MusicFade fade;

  fade.begin(music, THE_WHOLE_FADE);

  EXPECT_TRUE(music.playingOffset().has_value());
  EXPECT_EQ(music.fade(), SILENT);
}

/**
 * Given a fade of 3 s begun on a music
 * When 1.5 s pass
 * Then the music plays at half its volume
 */
TEST(MusicFade, HalfwayThroughItsFadeAMusicPlaysAtHalfItsVolume) {
  FakeMusicPlayer music;
  MusicFade fade;
  fade.begin(music, THE_WHOLE_FADE);

  fade.advance(HALF_THE_FADE);

  EXPECT_EQ(music.fade(), HALF_THE_VOLUME);
}

/**
 * Given a fade of 3 s begun on a music
 * When 1.5 s pass twice
 * Then the music plays at its full volume
 */
TEST(MusicFade, AFadeEndsAtTheFullVolume) {
  FakeMusicPlayer music;
  MusicFade fade;
  fade.begin(music, THE_WHOLE_FADE);
  fade.advance(HALF_THE_FADE);

  fade.advance(HALF_THE_FADE);

  EXPECT_EQ(music.fade(), THE_FULL_VOLUME);
}

/**
 * Given a fade of 3 s just begun on a music
 * When it is cancelled
 * Then the music plays at its full volume at once
 */
TEST(MusicFade, ACancelledFadeGivesTheFullVolumeBack) {
  FakeMusicPlayer music;
  MusicFade fade;
  fade.begin(music, THE_WHOLE_FADE);

  fade.cancel();

  EXPECT_EQ(music.fade(), THE_FULL_VOLUME);
}
