#include <gtest/gtest.h>
#include "NullMusicPlayer.hpp"

using rtype::client::NullMusicPlayer;

/**
 * Given a music that plays nothing
 * When it is started
 * Then it reports no position, so whoever follows it follows the frames
 */
TEST(NullMusicPlayer, StartedItStillHasNoPosition) {
  NullMusicPlayer music;

  music.start();

  EXPECT_FALSE(music.playingOffset().has_value());
}
