#include <gtest/gtest.h>
#include <SFML/Audio/InputSoundFile.hpp>
#include <SFML/System/Time.hpp>
#include <chrono>
#include <filesystem>
#include "AudioConstants.hpp"
#include "AudioTestConstants.hpp"

using rtype::client::GAME_MUSIC_ID;
using rtype::client::GAME_TRACK;
using rtype::client::testing::GAME_MUSIC_FADE_START;

/**
 * Given the music of a game in the repository
 * When it is opened, as the music is
 * Then it can be read, and it lasts longer than its loop
 */
TEST(GameMusic, TheMusicOfAGameHoldsItsLoop) {
  sf::InputSoundFile music;

  ASSERT_TRUE(music.openFromFile(
      std::filesystem::path(RTYPE_SOURCE_ASSETS_FOLDER) / GAME_MUSIC_ID));
  EXPECT_GT(
      music.getDuration().asSeconds(),
      std::chrono::duration<float>(GAME_TRACK.loopStart + GAME_TRACK.loopLength)
          .count());
}

/**
 * Given the part of the music of a game that loops
 * When its end is compared with the fade out of the file
 * Then it ends before the fade, so the loop never plays it
 */
TEST(GameMusic, TheLoopEndsBeforeTheFadeOut) {
  EXPECT_LT(
      std::chrono::duration<float>(GAME_TRACK.loopStart + GAME_TRACK.loopLength)
          .count(),
      GAME_MUSIC_FADE_START);
}
