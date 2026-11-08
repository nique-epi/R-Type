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
 * Then the music is silenced
 */
TEST_F(BossPresentationFixture, TheArrivalSilencesTheMusic) {
  presentation().apply(PRESENTED_BOSS_ARRIVING);

  EXPECT_EQ(audio().musicCalls(), std::vector<MusicCall>{MusicCall::Silence});
}

/**
 * Given the boss arriving, the music silenced
 * When the first status of its fight arrives
 * Then the music of the game is played again
 */
TEST_F(BossPresentationFixture, TheFightBringsTheMusicBack) {
  presentation().apply(PRESENTED_BOSS_ARRIVING);

  presentation().apply(PRESENTED_BOSS_FIGHTING);

  EXPECT_EQ(audio().musicCalls(),
            (std::vector<MusicCall>{MusicCall::Silence, MusicCall::GameMusic}));
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
            (std::vector<MusicCall>{MusicCall::Silence, MusicCall::GameMusic}));
}

/**
 * Given the boss stage started
 * When the time reaches the first step of the boss walking in
 * Then the sound of a step plays, once
 */
TEST_F(BossPresentationFixture, EachStepPlaysItsSound) {
  presentation().apply(PRESENTED_BOSS_ARRIVING);

  presentation().advance(TO_THE_FIRST_STEP);

  EXPECT_EQ(sounds().played(), std::vector<std::string>{HEAVY_IMPACT_SOUND_ID});
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
