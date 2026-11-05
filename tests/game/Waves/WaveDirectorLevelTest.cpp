#include <gtest/gtest.h>
#include <cstddef>
#include "WaveDirector.hpp"
#include "WaveDirectorLevelFixture.hpp"
#include "WaveDirectorLevelTestConstants.hpp"
#include "WaveDirectorTestConstants.hpp"
#include "WaveTestConstants.hpp"

using rtype::game::testing::A_MINUTE_OF_TICKS;
using rtype::game::testing::FIRST_WAVE_NUMBER_IN_THE_LOG;
using rtype::game::testing::NO_BYDO_IN_PLAY;
using rtype::game::testing::ONE_BYDO_IN_PLAY;
using rtype::game::testing::SECOND_WAVE_NUMBER_IN_THE_LOG;
using rtype::game::testing::TICKS_TO_THE_WHOLE_FIRST_WAVE;
using rtype::game::testing::TICKS_TO_THE_WHOLE_SECOND_WAVE;
using rtype::game::testing::WaveDirectorLevelFixture;

/**
 * Given a new wave director
 * When it is asked whether the last wave is finished
 * Then it is not
 */
TEST_F(WaveDirectorLevelFixture, TheLastWaveIsNotFinishedAtTheStart) {
  EXPECT_FALSE(oneWaveLevel().lastWaveFinished());
}

/**
 * Given a level of one wave, every Bydo of it due, one still in the world
 * When a minute of ticks runs
 * Then the last wave is not finished: it ends once the sky is clear
 */
TEST_F(WaveDirectorLevelFixture, TheLastWaveIsNotFinishedWhileABydoIsInPlay) {
  static_cast<void>(runTicks(oneWaveLevel(), TICKS_TO_THE_WHOLE_FIRST_WAVE,
                             ONE_BYDO_IN_PLAY));

  static_cast<void>(
      runTicks(oneWaveLevel(), A_MINUTE_OF_TICKS, ONE_BYDO_IN_PLAY));

  EXPECT_FALSE(oneWaveLevel().lastWaveFinished());
}

/**
 * Given a level of one wave, with no Bydo in the world, the last Bydo of the
 * wave not due yet
 * When the ticks before its due tick run
 * Then the last wave is not finished: a wave does not end before all its Bydos
 * came
 */
TEST_F(WaveDirectorLevelFixture, TheLastWaveWaitsForItsLastBydoToAppear) {
  static_cast<void>(runTicks(oneWaveLevel(), TICKS_TO_THE_WHOLE_FIRST_WAVE - 1,
                             NO_BYDO_IN_PLAY));

  EXPECT_FALSE(oneWaveLevel().lastWaveFinished());
}

/**
 * Given a level of one wave, every Bydo of it due and gone
 * When a tick finds no Bydo in the world
 * Then the last wave is finished, and the director still names the first wave
 */
TEST_F(WaveDirectorLevelFixture, TheLastWaveIsFinishedOnceTheSkyIsClear) {
  static_cast<void>(runTicks(oneWaveLevel(), TICKS_TO_THE_WHOLE_FIRST_WAVE,
                             ONE_BYDO_IN_PLAY));

  static_cast<void>(runTicks(oneWaveLevel(), 1, NO_BYDO_IN_PLAY));

  EXPECT_TRUE(oneWaveLevel().lastWaveFinished());
  EXPECT_EQ(oneWaveLevel().waveNumber(), FIRST_WAVE_NUMBER_IN_THE_LOG);
}

/**
 * Given the last wave of a level finished
 * When a minute of ticks runs with no Bydo in the world
 * Then no Bydo is due and the last wave stays finished
 */
TEST_F(WaveDirectorLevelFixture, NoBydoComesAfterTheLastWave) {
  static_cast<void>(runTicks(oneWaveLevel(), TICKS_TO_THE_WHOLE_FIRST_WAVE,
                             ONE_BYDO_IN_PLAY));
  static_cast<void>(runTicks(oneWaveLevel(), 1, NO_BYDO_IN_PLAY));

  EXPECT_EQ(runTicks(oneWaveLevel(), A_MINUTE_OF_TICKS, NO_BYDO_IN_PLAY), 0U);
  EXPECT_TRUE(oneWaveLevel().lastWaveFinished());
  EXPECT_EQ(oneWaveLevel().waveNumber(), FIRST_WAVE_NUMBER_IN_THE_LOG);
}

/**
 * Given a level of two waves, every Bydo of the first due and gone
 * When a tick finds no Bydo in the world
 * Then the second wave is next and the last wave is not finished
 */
TEST_F(WaveDirectorLevelFixture, AWaveBeforeTheLastDoesNotEndTheLevel) {
  static_cast<void>(runTicks(twoWaveLevel(), TICKS_TO_THE_WHOLE_FIRST_WAVE,
                             ONE_BYDO_IN_PLAY));

  static_cast<void>(runTicks(twoWaveLevel(), 1, NO_BYDO_IN_PLAY));

  EXPECT_FALSE(twoWaveLevel().lastWaveFinished());
  EXPECT_EQ(twoWaveLevel().waveNumber(), SECOND_WAVE_NUMBER_IN_THE_LOG);
}

/**
 * Given a level of two waves, the second wave played to its end
 * When a tick finds no Bydo in the world
 * Then the last wave is finished, and the director still names the second wave
 */
TEST_F(WaveDirectorLevelFixture, TheSecondWaveIsTheLastOfALevelOfTwo) {
  static_cast<void>(runTicks(twoWaveLevel(), TICKS_TO_THE_WHOLE_FIRST_WAVE,
                             ONE_BYDO_IN_PLAY));
  static_cast<void>(runTicks(twoWaveLevel(), 1, NO_BYDO_IN_PLAY));
  static_cast<void>(runTicks(twoWaveLevel(), TICKS_TO_THE_WHOLE_SECOND_WAVE,
                             ONE_BYDO_IN_PLAY));

  static_cast<void>(runTicks(twoWaveLevel(), 1, NO_BYDO_IN_PLAY));

  EXPECT_TRUE(twoWaveLevel().lastWaveFinished());
  EXPECT_EQ(twoWaveLevel().waveNumber(), SECOND_WAVE_NUMBER_IN_THE_LOG);
}
