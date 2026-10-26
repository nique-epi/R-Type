#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "GameException.hpp"
#include "WaveDirector.hpp"
#include "WaveDirectorFixture.hpp"
#include "WaveDirectorTestConstants.hpp"
#include "WaveProgressionTestConstants.hpp"
#include "WaveTestConstants.hpp"

using rtype::game::InvalidWaveCurveException;
using rtype::game::WaveDirector;
using rtype::game::testing::A_MINUTE_OF_TICKS;
using rtype::game::testing::CURVE_WITHOUT_BYDOS;
using rtype::game::testing::FIRST_WAVE_NUMBER_IN_THE_LOG;
using rtype::game::testing::HALF_A_SECOND_OF_TICKS;
using rtype::game::testing::NO_BYDO_IN_PLAY;
using rtype::game::testing::ONE_BYDO_IN_PLAY;
using rtype::game::testing::SECOND_WAVE_NUMBER_IN_THE_LOG;
using rtype::game::testing::TEST_SECOND_WAVE;
using rtype::game::testing::TEST_WAVE_CURVE;
using rtype::game::testing::TICKS_BETWEEN_TWO_BYDOS_OF_THE_FIRST_WAVE;
using rtype::game::testing::TICKS_BETWEEN_TWO_BYDOS_OF_THE_SECOND_WAVE;
using rtype::game::testing::TICKS_OF_A_PAUSE;
using rtype::game::testing::WaveDirectorFixture;

/**
 * Given a new wave director on the test curve
 * When its wave is asked for
 * Then it names the first wave of the curve
 */
TEST_F(WaveDirectorFixture, StartsWithTheFirstWave) {
  EXPECT_EQ(director().waveNumber(), FIRST_WAVE_NUMBER_IN_THE_LOG);
  EXPECT_EQ(director().currentWave(), TEST_WAVE_CURVE.firstWave);
}

/**
 * Given a new wave director and no Bydo in the world
 * When ticks run
 * Then the first Bydo is due at the end of the first pause, on the 241st tick
 */
TEST_F(WaveDirectorFixture, TheFirstBydoComesAtTheEndOfTheFirstPause) {
  EXPECT_EQ(ticksToTheNextBydo(NO_BYDO_IN_PLAY), TICKS_OF_A_PAUSE);
}

/**
 * Given a new wave director
 * When ticks run while the Bydos of the first wave come into the world
 * Then they are due one by one: the first after the pause, the next two 2
 * seconds apart
 */
TEST_F(WaveDirectorFixture, TheBydosOfTheFirstWaveComeTwoSecondsApart) {
  const std::vector<std::size_t> ticks{ticksToTheNextBydo(NO_BYDO_IN_PLAY),
                                       ticksToTheNextBydo(ONE_BYDO_IN_PLAY),
                                       ticksToTheNextBydo(ONE_BYDO_IN_PLAY)};

  EXPECT_EQ(ticks, (std::vector{TICKS_OF_A_PAUSE,
                                TICKS_BETWEEN_TWO_BYDOS_OF_THE_FIRST_WAVE,
                                TICKS_BETWEEN_TWO_BYDOS_OF_THE_FIRST_WAVE}));
}

/**
 * Given the first Bydo of the first wave, already gone from the world
 * When ticks run with no Bydo in the world
 * Then the second Bydo of the wave is still due 2 seconds after the first: a
 * wave does not end before all its Bydos came
 */
TEST_F(WaveDirectorFixture, ABydoGoneEarlyDoesNotEndItsWave) {
  static_cast<void>(ticksToTheNextBydo(NO_BYDO_IN_PLAY));

  EXPECT_EQ(ticksToTheNextBydo(NO_BYDO_IN_PLAY),
            TICKS_BETWEEN_TWO_BYDOS_OF_THE_FIRST_WAVE);
}

/**
 * Given every Bydo of the first wave due
 * When a minute of ticks runs with one Bydo still in the world
 * Then no Bydo is due: the next wave waits for the sky to be clear
 */
TEST_F(WaveDirectorFixture, TheNextWaveWaitsWhileABydoIsInPlay) {
  letTheFirstWaveAppear();

  EXPECT_EQ(runTicksWithABydoInPlay(A_MINUTE_OF_TICKS), 0U);
}

/**
 * Given every Bydo of the first wave due, one of them still in the world half
 * a second later
 * When ticks run with no Bydo in the world
 * Then the next Bydo is due a whole pause after the first of these ticks, on
 * its 241st tick
 */
TEST_F(WaveDirectorFixture, TheNextWaveComesAPauseAfterTheSkyIsClear) {
  letTheFirstWaveAppear();
  static_cast<void>(runTicksWithABydoInPlay(HALF_A_SECOND_OF_TICKS));

  EXPECT_EQ(ticksToTheNextBydo(NO_BYDO_IN_PLAY), TICKS_OF_A_PAUSE);
}

/**
 * Given every Bydo of the first wave due, then gone
 * When the second wave comes, its Bydos staying in the world
 * Then it brings 4 Bydos, the first after the pause and the next ones 1.75
 * seconds apart, and no fifth
 */
TEST_F(WaveDirectorFixture, TheSecondWaveBringsMoreBydosCloserTogether) {
  letTheFirstWaveAppear();

  const std::vector<std::size_t> ticks{ticksToTheNextBydo(NO_BYDO_IN_PLAY),
                                       ticksToTheNextBydo(ONE_BYDO_IN_PLAY),
                                       ticksToTheNextBydo(ONE_BYDO_IN_PLAY),
                                       ticksToTheNextBydo(ONE_BYDO_IN_PLAY)};

  EXPECT_EQ(ticks, (std::vector{TICKS_OF_A_PAUSE,
                                TICKS_BETWEEN_TWO_BYDOS_OF_THE_SECOND_WAVE,
                                TICKS_BETWEEN_TWO_BYDOS_OF_THE_SECOND_WAVE,
                                TICKS_BETWEEN_TWO_BYDOS_OF_THE_SECOND_WAVE}));
  EXPECT_EQ(runTicksWithABydoInPlay(A_MINUTE_OF_TICKS), 0U);
}

/**
 * Given every Bydo of the first wave due
 * When a tick finds no Bydo in the world
 * Then the director names the second wave of the curve
 */
TEST_F(WaveDirectorFixture, MovesOnToTheSecondWaveOnceTheSkyIsClear) {
  letTheFirstWaveAppear();

  runATickWithTheSkyClear();

  EXPECT_EQ(director().waveNumber(), SECOND_WAVE_NUMBER_IN_THE_LOG);
  EXPECT_EQ(director().currentWave(), TEST_SECOND_WAVE);
}

/**
 * Given a curve whose first wave holds no Bydo
 * When a wave director is built on it
 * Then an InvalidWaveCurveException is thrown
 */
TEST(WaveDirector, RefusesACurveThatIsNotPlayable) {
  EXPECT_THROW(static_cast<void>(WaveDirector{CURVE_WITHOUT_BYDOS}),
               InvalidWaveCurveException);
}
