#include <gtest/gtest.h>
#include "AudioTestConstants.hpp"
#include "FakeMusicPlayer.hpp"
#include "Soundtrack.hpp"
#include "SoundtrackParts.hpp"

using rtype::client::Soundtrack;
using rtype::client::SoundtrackParts;
using rtype::client::testing::HALF_THE_FADE;
using rtype::client::testing::HALF_THE_VOLUME;
using rtype::client::testing::JUST_BEFORE_THE_MUSIC_OF_A_BOSS_ENTERS;
using rtype::client::testing::LONGER_THAN_THE_SILENCE;
using rtype::client::testing::MOST_OF_THE_SILENCE;
using rtype::client::testing::PART_OF_THE_SILENCE;
using rtype::client::testing::REST_OF_THE_SILENCE;
using rtype::client::testing::REST_OF_THE_WAIT;
using rtype::client::testing::SILENT;
using rtype::client::testing::THE_FULL_VOLUME;
using rtype::client::testing::THE_WHOLE_ALARM;
using rtype::client::testing::THE_WHOLE_FADE;
using rtype::client::testing::THE_WHOLE_SILENCE;
using rtype::client::testing::UNTIL_THE_FIGHT;
using rtype::client::testing::UNTIL_THE_MUSIC_OF_A_BOSS_ENTERS;
using rtype::client::testing::WELL_INTO_THE_THEME;

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
 * When the music of the boss is asked for and the alarm plays on until just
 * before its last 3 s
 * Then the music of the game and the theme stopped at once, and the boss
 * waits for the tail of its alarm
 */
TEST(Soundtrack, TheMusicOfTheBossWaitsForTheTailOfTheAlarm) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  FakeMusicPlayer bossMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
  soundtrack.playGameMusic();
  soundtrack.advance(THE_WHOLE_SILENCE);

  soundtrack.playBossMusic(false);
  soundtrack.advance(JUST_BEFORE_THE_MUSIC_OF_A_BOSS_ENTERS);

  EXPECT_FALSE(gameMusic.playingOffset().has_value());
  EXPECT_FALSE(theme.playingOffset().has_value());
  EXPECT_FALSE(bossMusic.playingOffset().has_value());
}

/**
 * Given the music of the boss asked for
 * When the alarm reaches its last 3 s
 * Then the music of the boss starts, silent
 */
TEST(Soundtrack, TheMusicOfTheBossEntersSilentOverTheTailOfTheAlarm) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  FakeMusicPlayer bossMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
  soundtrack.playBossMusic(false);

  soundtrack.advance(UNTIL_THE_MUSIC_OF_A_BOSS_ENTERS);

  EXPECT_TRUE(bossMusic.playingOffset().has_value());
  EXPECT_EQ(bossMusic.fade(), SILENT);
}

/**
 * Given the music of the boss entered over the tail of the alarm
 * When half of its fade passes
 * Then it plays at half its volume
 */
TEST(Soundtrack, HalfwayThroughItsFadeTheMusicOfTheBossIsAtHalfItsVolume) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  FakeMusicPlayer bossMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
  soundtrack.playBossMusic(false);
  soundtrack.advance(UNTIL_THE_MUSIC_OF_A_BOSS_ENTERS);

  soundtrack.advance(HALF_THE_FADE);

  EXPECT_EQ(bossMusic.fade(), HALF_THE_VOLUME);
}

/**
 * Given the music of the boss entered over the tail of the alarm
 * When the rest of the alarm passes
 * Then it plays at its full volume as the alarm ends
 */
TEST(Soundtrack, TheMusicOfTheBossIsAtItsFullVolumeWhenTheAlarmEnds) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  FakeMusicPlayer bossMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
  soundtrack.playBossMusic(false);
  soundtrack.advance(UNTIL_THE_MUSIC_OF_A_BOSS_ENTERS);

  soundtrack.advance(THE_WHOLE_FADE);

  EXPECT_EQ(bossMusic.fade(), THE_FULL_VOLUME);
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
  soundtrack.playBossMusic(false);
  soundtrack.advance(THE_WHOLE_ALARM);

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
  soundtrack.playBossMusic(false);
  soundtrack.playTheme();

  soundtrack.advance(THE_WHOLE_ALARM);

  EXPECT_FALSE(bossMusic.playingOffset().has_value());
}

/**
 * Given the theme playing
 * When a screen with a music of its own pauses it
 * Then the theme is silent
 */
TEST(Soundtrack, APausedThemeIsSilent) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  FakeMusicPlayer bossMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
  theme.start();

  soundtrack.pauseTheme();

  EXPECT_FALSE(theme.playingOffset().has_value());
}

/**
 * Given the theme paused 42 s into it
 * When it is resumed
 * Then it plays on from 42 s, not from its start
 */
TEST(Soundtrack, AResumedThemePlaysOnFromWhereItWas) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  FakeMusicPlayer bossMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
  theme.start();
  theme.setPlayingOffset(WELL_INTO_THE_THEME);
  soundtrack.pauseTheme();

  soundtrack.resumeTheme();

  EXPECT_EQ(theme.playingOffset(), WELL_INTO_THE_THEME);
}

/**
 * Given the theme paused
 * When the theme is asked for, as a title screen below does each frame
 * Then it stays paused
 */
TEST(Soundtrack, AskingForAPausedThemeLeavesItPaused) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  FakeMusicPlayer bossMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
  theme.start();
  soundtrack.pauseTheme();

  soundtrack.playTheme();

  EXPECT_FALSE(theme.playingOffset().has_value());
}

/**
 * Given the theme paused
 * When a game starts, then the theme is resumed
 * Then the theme stays silent: the game stopped it for good
 */
TEST(Soundtrack, AGameStopsAPausedThemeForGood) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  FakeMusicPlayer bossMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
  theme.start();
  soundtrack.pauseTheme();
  soundtrack.playGameMusic();

  soundtrack.resumeTheme();

  EXPECT_FALSE(theme.playingOffset().has_value());
}

/**
 * Given a theme that does not play
 * When it is paused, then resumed
 * Then it still does not play: resuming starts only what was paused
 */
TEST(Soundtrack, ResumingAThemeThatDidNotPlayStartsNothing) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  FakeMusicPlayer bossMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
  soundtrack.pauseTheme();

  soundtrack.resumeTheme();

  EXPECT_FALSE(theme.playingOffset().has_value());
}

/**
 * Given the music of a game about to start, its silence running
 * When the level names a music of its own, then the whole silence passes
 * Then the music of the level plays, and the music of a game does not
 */
TEST(Soundtrack, TheMusicOfALevelPlaysInPlaceOfTheMusicOfAGame) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  FakeMusicPlayer bossMusic;
  FakeMusicPlayer levelMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
  soundtrack.playGameMusic();

  soundtrack.useLevelMusic(levelMusic);
  soundtrack.advance(LONGER_THAN_THE_SILENCE);

  EXPECT_TRUE(levelMusic.playingOffset().has_value());
  EXPECT_FALSE(gameMusic.playingOffset().has_value());
}

/**
 * Given the music of a game that plays
 * When the level names a music of its own
 * Then the music of the level plays at once, in place of the music of a game
 */
TEST(Soundtrack, TheMusicOfALevelTakesOverTheMusicThatPlays) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  FakeMusicPlayer bossMusic;
  FakeMusicPlayer levelMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
  soundtrack.playGameMusic();
  soundtrack.advance(LONGER_THAN_THE_SILENCE);

  soundtrack.useLevelMusic(levelMusic);

  EXPECT_TRUE(levelMusic.playingOffset().has_value());
  EXPECT_FALSE(gameMusic.playingOffset().has_value());
}

/**
 * Given the music of a level that plays
 * When a boss arrives and silences the music
 * Then the music of the level stops, for the alarm to be heard
 */
TEST(Soundtrack, TheArrivalSilencesTheMusicOfALevel) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  FakeMusicPlayer bossMusic;
  FakeMusicPlayer levelMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
  soundtrack.playGameMusic();
  soundtrack.useLevelMusic(levelMusic);
  soundtrack.advance(LONGER_THAN_THE_SILENCE);

  soundtrack.silenceMusic();

  EXPECT_FALSE(levelMusic.playingOffset().has_value());
}

/**
 * Given the music of a level silenced by a boss whose sheet names no music
 * When the alarm reaches its last 3 s
 * Then the music of the level comes back in its place, and the music of a
 * boss stays silent
 */
TEST(Soundtrack, ABossWithoutMusicGivesTheLevelItsMusicBack) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  FakeMusicPlayer bossMusic;
  FakeMusicPlayer levelMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
  soundtrack.playGameMusic();
  soundtrack.useLevelMusic(levelMusic);
  soundtrack.advance(LONGER_THAN_THE_SILENCE);
  soundtrack.silenceMusic();
  soundtrack.playBossMusic(false);

  soundtrack.advance(UNTIL_THE_MUSIC_OF_A_BOSS_ENTERS);

  EXPECT_TRUE(levelMusic.playingOffset().has_value());
  EXPECT_FALSE(bossMusic.playingOffset().has_value());
}

/**
 * Given the music of a level silenced by a boss, and its sheet read during
 * the alarm, naming a music of its own
 * When the alarm reaches its last 3 s
 * Then the music of the boss comes in, and the music of the level stays
 * silent
 */
TEST(Soundtrack, ASheetReadDuringTheAlarmBringsTheMusicOfTheBoss) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  FakeMusicPlayer bossMusic;
  FakeMusicPlayer levelMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
  soundtrack.playGameMusic();
  soundtrack.useLevelMusic(levelMusic);
  soundtrack.advance(LONGER_THAN_THE_SILENCE);
  soundtrack.silenceMusic();
  soundtrack.playBossMusic(false);
  soundtrack.advance(UNTIL_THE_FIGHT);

  soundtrack.playBossMusic(true);
  soundtrack.advance(REST_OF_THE_WAIT);

  EXPECT_TRUE(bossMusic.playingOffset().has_value());
  EXPECT_FALSE(levelMusic.playingOffset().has_value());
}

/**
 * Given the music of a level fading back in after the alarm of a boss with
 * no music of its own
 * When the boss is gone and the music of a game is asked for
 * Then the music of the level plays on at its full volume
 */
TEST(Soundtrack, TheBossGoneDuringTheFadeGivesTheLevelItsFullVolume) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  FakeMusicPlayer bossMusic;
  FakeMusicPlayer levelMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
  soundtrack.playGameMusic();
  soundtrack.useLevelMusic(levelMusic);
  soundtrack.advance(LONGER_THAN_THE_SILENCE);
  soundtrack.silenceMusic();
  soundtrack.playBossMusic(false);
  soundtrack.advance(UNTIL_THE_MUSIC_OF_A_BOSS_ENTERS);

  soundtrack.playGameMusic();

  EXPECT_TRUE(levelMusic.playingOffset().has_value());
  EXPECT_EQ(levelMusic.fade(), THE_FULL_VOLUME);
}

/**
 * Given the music of a level that played, then the theme of a title screen
 * When the next game starts and its whole silence passes
 * Then the music of a game plays, the music of the level forgotten
 */
TEST(Soundtrack, TheThemeForgetsTheMusicOfTheLevel) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  FakeMusicPlayer bossMusic;
  FakeMusicPlayer levelMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
  soundtrack.playGameMusic();
  soundtrack.useLevelMusic(levelMusic);
  soundtrack.advance(LONGER_THAN_THE_SILENCE);
  soundtrack.playTheme();

  soundtrack.playGameMusic();
  soundtrack.advance(LONGER_THAN_THE_SILENCE);

  EXPECT_TRUE(gameMusic.playingOffset().has_value());
  EXPECT_FALSE(levelMusic.playingOffset().has_value());
}

/**
 * Given the music of a level that plays again, the alarm of a boss over
 * When the sheet of the boss, read late, names a music of its own
 * Then the music of the boss plays at once, with no alarm to wait for
 */
TEST(Soundtrack, ASheetReadAfterTheAlarmStartsTheMusicOfTheBossAtOnce) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  FakeMusicPlayer bossMusic;
  FakeMusicPlayer levelMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
  soundtrack.playGameMusic();
  soundtrack.useLevelMusic(levelMusic);
  soundtrack.advance(LONGER_THAN_THE_SILENCE);

  soundtrack.playBossMusic(true);

  EXPECT_TRUE(bossMusic.playingOffset().has_value());
}

/**
 * Given the music of a level that plays again, the alarm of a boss over
 * When the sheet of the boss, read late, names a music of its own
 * Then the music of the level stops
 */
TEST(Soundtrack, TheMusicOfABossStopsTheMusicOfALevel) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  FakeMusicPlayer bossMusic;
  FakeMusicPlayer levelMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
  soundtrack.playGameMusic();
  soundtrack.useLevelMusic(levelMusic);
  soundtrack.advance(LONGER_THAN_THE_SILENCE);

  soundtrack.playBossMusic(true);

  EXPECT_FALSE(levelMusic.playingOffset().has_value());
}

/**
 * Given the music of a boss that took over the music of a level after its
 * alarm
 * When the boss is gone and the music of a game is asked for again
 * Then the music of the level plays again once the silence passed
 */
TEST(Soundtrack, TheMusicOfALevelComesBackAfterTheMusicOfABoss) {
  FakeMusicPlayer theme;
  FakeMusicPlayer gameMusic;
  FakeMusicPlayer bossMusic;
  FakeMusicPlayer levelMusic;
  Soundtrack soundtrack(SoundtrackParts{
      .theme = theme, .gameMusic = gameMusic, .bossMusic = bossMusic});
  soundtrack.playGameMusic();
  soundtrack.useLevelMusic(levelMusic);
  soundtrack.advance(LONGER_THAN_THE_SILENCE);
  soundtrack.silenceMusic();
  soundtrack.playBossMusic(true);
  soundtrack.advance(UNTIL_THE_MUSIC_OF_A_BOSS_ENTERS);

  soundtrack.playGameMusic();
  soundtrack.advance(LONGER_THAN_THE_SILENCE);

  EXPECT_TRUE(levelMusic.playingOffset().has_value());
}
