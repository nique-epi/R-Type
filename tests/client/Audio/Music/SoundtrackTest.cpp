#include <gtest/gtest.h>
#include "AudioTestConstants.hpp"
#include "FakeMusicPlayer.hpp"
#include "Soundtrack.hpp"
#include "SoundtrackParts.hpp"

using rtype::client::Soundtrack;
using rtype::client::SoundtrackParts;
using rtype::client::testing::LONGER_THAN_THE_SILENCE;
using rtype::client::testing::MOST_OF_THE_SILENCE;
using rtype::client::testing::PART_OF_THE_SILENCE;
using rtype::client::testing::REST_OF_THE_SILENCE;
using rtype::client::testing::THE_WHOLE_SILENCE;

/**
 * Given the theme playing
 * When a game starts
 * Then the theme stops at once
 */
TEST(Soundtrack, AGameStopsTheTheme) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  Soundtrack soundtrack(
      SoundtrackParts{.theme = theme, .gameMusic = gameMusic});
  theme.start();

  soundtrack.playGameMusic();

  EXPECT_FALSE(theme.playingOffset().has_value());
}

/**
 * Given a game just started
 * When most of a second passes
 * Then the music of the game has not started: the silence goes on
 */
TEST(Soundtrack, TheMusicOfAGameWaitsForTheSilence) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  Soundtrack soundtrack(
      SoundtrackParts{.theme = theme, .gameMusic = gameMusic});
  soundtrack.playGameMusic();

  soundtrack.advance(MOST_OF_THE_SILENCE);

  EXPECT_FALSE(gameMusic.playingOffset().has_value());
}

/**
 * Given a game just started
 * When a second of silence passes
 * Then the music of the game starts
 */
TEST(Soundtrack, TheMusicOfAGameStartsAfterASecondOfSilence) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  Soundtrack soundtrack(
      SoundtrackParts{.theme = theme, .gameMusic = gameMusic});
  soundtrack.playGameMusic();

  soundtrack.advance(THE_WHOLE_SILENCE);

  EXPECT_TRUE(gameMusic.playingOffset().has_value());
}

/**
 * Given a game started 0.6 s ago
 * When the game is asked for again, then 0.4 s more pass
 * Then the music of the game starts: asking again does not restart the
 * silence
 */
TEST(Soundtrack, AskingForTheGameAgainKeepsTheSilenceGoing) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  Soundtrack soundtrack(
      SoundtrackParts{.theme = theme, .gameMusic = gameMusic});
  soundtrack.playGameMusic();
  soundtrack.advance(PART_OF_THE_SILENCE);

  soundtrack.playGameMusic();
  soundtrack.advance(REST_OF_THE_SILENCE);

  EXPECT_TRUE(gameMusic.playingOffset().has_value());
}

/**
 * Given the music of a game playing
 * When a title screen asks for the theme
 * Then the music of the game stops
 */
TEST(Soundtrack, ATitleStopsTheMusicOfTheGame) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  Soundtrack soundtrack(
      SoundtrackParts{.theme = theme, .gameMusic = gameMusic});
  soundtrack.playGameMusic();
  soundtrack.advance(THE_WHOLE_SILENCE);

  soundtrack.playTheme();

  EXPECT_FALSE(gameMusic.playingOffset().has_value());
}

/**
 * Given the music of a game playing
 * When a title screen asks for the theme
 * Then the theme plays again
 */
TEST(Soundtrack, ATitleBringsTheThemeBack) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  Soundtrack soundtrack(
      SoundtrackParts{.theme = theme, .gameMusic = gameMusic});
  soundtrack.playGameMusic();
  soundtrack.advance(THE_WHOLE_SILENCE);

  soundtrack.playTheme();

  EXPECT_TRUE(theme.playingOffset().has_value());
}

/**
 * Given a game left for a title during the silence
 * When two seconds pass
 * Then the music of the game never starts
 */
TEST(Soundtrack, LeavingDuringTheSilenceCancelsTheMusicOfTheGame) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  Soundtrack soundtrack(
      SoundtrackParts{.theme = theme, .gameMusic = gameMusic});
  soundtrack.playGameMusic();
  soundtrack.playTheme();

  soundtrack.advance(LONGER_THAN_THE_SILENCE);

  EXPECT_FALSE(gameMusic.playingOffset().has_value());
}
