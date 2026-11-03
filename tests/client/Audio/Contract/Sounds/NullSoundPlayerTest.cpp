#include <gtest/gtest.h>
#include "AudioTestConstants.hpp"
#include "NullSoundPlayer.hpp"

using rtype::client::NullSoundPlayer;
using rtype::client::testing::UNKNOWN_SOUND_ID;

/**
 * Given a sound player that plays nothing
 * When it is asked to play a sound no asset has
 * Then it accepts the call without failing
 */
TEST(NullSoundPlayer, AcceptsASoundNoAssetHas) {
  NullSoundPlayer sounds;

  EXPECT_NO_THROW(sounds.play(UNKNOWN_SOUND_ID));
}
