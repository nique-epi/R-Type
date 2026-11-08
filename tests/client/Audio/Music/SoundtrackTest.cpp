#include <gtest/gtest.h>
#include <chrono>
#include "AudioConstants.hpp"
#include "AudioTestConstants.hpp"
#include "FakeMusicPlayer.hpp"
#include "Soundtrack.hpp"
#include "SoundtrackParts.hpp"

using rtype::client::BOSS_ALARM_DURATION;
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
  FakeMusicPlayer bossMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
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
  FakeMusicPlayer bossMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
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
  FakeMusicPlayer bossMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
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
  FakeMusicPlayer bossMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
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
  FakeMusicPlayer bossMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
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
  FakeMusicPlayer bossMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
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
  FakeMusicPlayer bossMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
  soundtrack.playGameMusic();
  soundtrack.playTheme();

  soundtrack.advance(LONGER_THAN_THE_SILENCE);

  EXPECT_FALSE(gameMusic.playingOffset().has_value());
}

/**
 * Given the music of a game playing
 * When the music is silenced
 * Then it stops at once
 */
TEST(Soundtrack, SilencingStopsTheMusicOfAGameAtOnce) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  FakeMusicPlayer bossMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
  soundtrack.playGameMusic();
  soundtrack.advance(LONGER_THAN_THE_SILENCE);

  soundtrack.silenceMusic();

  EXPECT_FALSE(gameMusic.playingOffset().has_value());
}

/**
 * Given the music of a game about to start, its silence running
 * When the music is silenced, then the rest of the silence passes
 * Then it does not start: the silence was cancelled
 */
TEST(Soundtrack, SilencingCancelsTheMusicAboutToStart) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  FakeMusicPlayer bossMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
  soundtrack.playGameMusic();
  soundtrack.advance(PART_OF_THE_SILENCE);

  soundtrack.silenceMusic();
  soundtrack.advance(LONGER_THAN_THE_SILENCE);

  EXPECT_FALSE(gameMusic.playingOffset().has_value());
}

/**
 * Given the music of a game silenced
 * When it is asked again and the whole silence passes
 * Then it plays again
 */
TEST(Soundtrack, TheMusicOfAGameComesBackAfterBeingSilenced) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  FakeMusicPlayer bossMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
  soundtrack.playGameMusic();
  soundtrack.advance(LONGER_THAN_THE_SILENCE);
  soundtrack.silenceMusic();

  soundtrack.playGameMusic();
  soundtrack.advance(THE_WHOLE_SILENCE);

  EXPECT_TRUE(gameMusic.playingOffset().has_value());
}

/**
 * Given the music of a game playing
 * When the music of the boss is asked for
 * Then the music of the game and the theme stop at once, and the boss waits
 * for its alarm
 */
TEST(Soundtrack, TheMusicOfTheBossWaitsForTheAlarm) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  FakeMusicPlayer bossMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
  soundtrack.playGameMusic();
  soundtrack.advance(THE_WHOLE_SILENCE);

  soundtrack.playBossMusic();
  soundtrack.advance(BOSS_ALARM_DURATION - std::chrono::milliseconds(1));

  EXPECT_FALSE(gameMusic.playingOffset().has_value());
  EXPECT_FALSE(theme.playingOffset().has_value());
  EXPECT_FALSE(bossMusic.playingOffset().has_value());
}

/**
 * Given the music of the boss asked for
 * When the whole alarm has played
 * Then the music of the boss starts
 */
TEST(Soundtrack, TheMusicOfTheBossStartsWhenTheAlarmEnds) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  FakeMusicPlayer bossMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
  soundtrack.playBossMusic();

  soundtrack.advance(BOSS_ALARM_DURATION);

  EXPECT_TRUE(bossMusic.playingOffset().has_value());
}

/**
 * Given the music of the boss playing
 * When a game starts again
 * Then the music of the boss stops
 */
TEST(Soundtrack, AGameStopsTheMusicOfTheBoss) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  FakeMusicPlayer bossMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
  soundtrack.playBossMusic();
  soundtrack.advance(BOSS_ALARM_DURATION);

  soundtrack.playGameMusic();

  EXPECT_FALSE(bossMusic.playingOffset().has_value());
}

/**
 * Given the music of the boss asked for, its alarm still playing
 * When a title screen asks for the theme
 * Then the music of the boss never starts
 */
TEST(Soundtrack, LeavingDuringTheAlarmCancelsTheMusicOfTheBoss) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  FakeMusicPlayer bossMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
  soundtrack.playBossMusic();
  soundtrack.playTheme();

  soundtrack.advance(BOSS_ALARM_DURATION);

  EXPECT_FALSE(bossMusic.playingOffset().has_value());
}
