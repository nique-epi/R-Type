#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "BossLook.hpp"
#include "BossPresentationFixture.hpp"
#include "BossPresentationTestConstants.hpp"
#include "FlashPreference.hpp"
#include "MusicCall.hpp"
#include "SoundConstants.hpp"

using rtype::client::BOSS_ALARM_SOUND_ID;
using rtype::client::BossLook;
using rtype::client::BYDO_GROWL_SOUND_ID;
using rtype::client::FlashPreference;
using rtype::client::HEAVY_IMPACT_SOUND_ID;
using ::testing::Contains;
using ::testing::Not;

/**
 * Given a presentation told nothing
 * When its look is read
 * Then it lays nothing over the playfield
 */
TEST_F(BossPresentationFixture, TheLookIsEmptyBeforeAnyArrival) {
  EXPECT_EQ(presentation().look(FlashPreference::Full), BossLook{});
}

/**
 * Given the music of a game playing
 * When the boss stage starts
 * Then the music of the game is silenced before the music of the boss is
 * asked for
 */
TEST_F(BossPresentationFixture, TheArrivalSilencesTheMusic) {
  presentation().apply(PRESENTED_BOSS_ARRIVING);

  ASSERT_GE(audio().musicCalls().size(), 1U);
  EXPECT_EQ(audio().musicCalls().front(), MusicCall::Silence);
}

/**
 * Given a presentation told nothing
 * When the boss starts to arrive
 * Then the alarm plays once, and the music of the boss is asked for after
 * the music of the game is silenced
 */
TEST_F(BossPresentationFixture, TheArrivalPlaysTheAlarmThenAsksForBossMusic) {
  presentation().apply(PRESENTED_BOSS_ARRIVING);
  presentation().apply(PRESENTED_BOSS_ARRIVING);

  EXPECT_EQ(sounds().played(), std::vector<std::string>{BOSS_ALARM_SOUND_ID});
  EXPECT_EQ(audio().musicCalls(),
            (std::vector<MusicCall>{MusicCall::Silence, MusicCall::BossMusic}));
}

/**
 * Given the boss arriving, its music asked for
 * When the first status of its fight arrives
 * Then the music of the game does not come back: the boss keeps its music
 */
TEST_F(BossPresentationFixture, TheFightKeepsTheMusicOfTheBoss) {
  presentation().apply(PRESENTED_BOSS_ARRIVING);

  presentation().apply(PRESENTED_BOSS_FIGHTING);

  EXPECT_EQ(audio().musicCalls(),
            (std::vector<MusicCall>{MusicCall::Silence, MusicCall::BossMusic}));
}

/**
 * Given the boss fighting, its music playing
 * When the game ends
 * Then the music of the game is played again
 */
TEST_F(BossPresentationFixture, AGameEndedAfterTheFightBringsTheMusicBack) {
  presentation().apply(PRESENTED_BOSS_ARRIVING);
  presentation().apply(PRESENTED_BOSS_FIGHTING);

  presentation().endGame();

  EXPECT_EQ(audio().musicCalls(),
            (std::vector<MusicCall>{MusicCall::Silence, MusicCall::BossMusic,
                                    MusicCall::GameMusic}));
}

/**
 * Given a game that plays a level, its boss fighting, its music playing
 * When it is defeated, while the level goes on
 * Then the music of the game is played again
 */
TEST_F(BossPresentationFixture, ADefeatedBossOfALevelBringsTheMusicBack) {
  presentation().followLevel();
  presentation().apply(PRESENTED_BOSS_ARRIVING);
  presentation().apply(PRESENTED_BOSS_FIGHTING);

  presentation().apply(PRESENTED_BOSS_DEFEATED);

  EXPECT_EQ(audio().musicCalls(),
            (std::vector<MusicCall>{MusicCall::Silence, MusicCall::BossMusic,
                                    MusicCall::GameMusic}));
}

/**
 * Given a game of waves, its boss fighting, its music playing
 * When it is defeated
 * Then the music of the boss goes on until the game ends
 */
TEST_F(BossPresentationFixture, ADefeatedBossOfTheWavesKeepsItsMusic) {
  presentation().apply(PRESENTED_BOSS_ARRIVING);
  presentation().apply(PRESENTED_BOSS_FIGHTING);

  presentation().apply(PRESENTED_BOSS_DEFEATED);

  EXPECT_EQ(audio().musicCalls(),
            (std::vector<MusicCall>{MusicCall::Silence, MusicCall::BossMusic}));
}

/**
 * Given a game that plays a level, a first boss defeated
 * When a second boss arrives
 * Then the alarm plays again, and its music is asked for again
 */
TEST_F(BossPresentationFixture, ASecondBossOfALevelSoundsTheAlarmAgain) {
  presentation().followLevel();
  presentation().apply(PRESENTED_BOSS_ARRIVING);
  presentation().apply(PRESENTED_BOSS_FIGHTING);
  presentation().apply(PRESENTED_BOSS_DEFEATED);

  presentation().apply(PRESENTED_BOSS_ARRIVING);

  EXPECT_EQ(sounds().played(), (std::vector<std::string>{BOSS_ALARM_SOUND_ID,
                                                         BOSS_ALARM_SOUND_ID}));
  EXPECT_EQ(audio().musicCalls(),
            (std::vector<MusicCall>{MusicCall::Silence, MusicCall::BossMusic,
                                    MusicCall::GameMusic, MusicCall::Silence,
                                    MusicCall::BossMusic}));
}

/**
 * Given the boss arriving, the music silenced
 * When the game ends
 * Then the music of the game is played again
 */
TEST_F(BossPresentationFixture, AGameEndedDuringTheArrivalBringsTheMusicBack) {
  presentation().apply(PRESENTED_BOSS_ARRIVING);

  presentation().endGame();

  EXPECT_EQ(audio().musicCalls(),
            (std::vector<MusicCall>{MusicCall::Silence, MusicCall::BossMusic,
                                    MusicCall::GameMusic}));
}

/**
 * Given the boss stage started
 * When the time reaches the first step of the boss walking in
 * Then the sound of a step plays, once
 */
TEST_F(BossPresentationFixture, EachStepPlaysItsSound) {
  presentation().apply(PRESENTED_BOSS_ARRIVING);

  presentation().advance(TO_THE_FIRST_STEP);

  EXPECT_THAT(sounds().played(), Contains(std::string{HEAVY_IMPACT_SOUND_ID}));
}

/**
 * Given the boss walked in during its arrival
 * When the strike of its attack 0 is announced
 * Then the sound of its roar plays
 */
TEST_F(BossPresentationFixture, TheRoarPlaysItsSound) {
  presentation().apply(PRESENTED_BOSS_ARRIVING);
  presentation().advance(TO_THE_ROAR);

  presentation().apply(PRESENTED_BOSS_ROAR);

  EXPECT_THAT(sounds().played(), Contains(std::string{BYDO_GROWL_SOUND_ID}));
}

/**
 * Given the boss in its fight
 * When the strike of an attack 0 is announced
 * Then no roar plays: the roar belongs to the arrival
 */
TEST_F(BossPresentationFixture, AnAttackZeroStrikeAfterTheArrivalIsNoRoar) {
  presentation().apply(PRESENTED_BOSS_ARRIVING);
  presentation().apply(PRESENTED_BOSS_FIGHTING);

  presentation().apply(PRESENTED_BOSS_ROAR);

  EXPECT_THAT(sounds().played(),
              Not(Contains(std::string{BYDO_GROWL_SOUND_ID})));
}

/**
 * Given the boss arriving
 * When the first status of its fight arrives
 * Then its bar shows
 */
TEST_F(BossPresentationFixture, TheBarShowsOnceTheBossFights) {
  presentation().apply(PRESENTED_BOSS_ARRIVING);

  presentation().apply(PRESENTED_BOSS_FIGHTING);

  EXPECT_TRUE(presentation().look(FlashPreference::Full).bar.shown);
}
