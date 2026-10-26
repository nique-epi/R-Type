#include <gtest/gtest.h>
#include <cstddef>
#include "BydoWave.hpp"
#include "WaveProgression.hpp"
#include "WaveProgressionTestConstants.hpp"
#include "WaveTestConstants.hpp"

using rtype::game::BydoWave;
using rtype::game::bydoWaveAfter;
using rtype::game::isPlayable;
using rtype::game::testing::CURVE_THAT_SLOWS_DOWN;
using rtype::game::testing::CURVE_WITH_A_NEGATIVE_CUT;
using rtype::game::testing::CURVE_WITH_A_NEGATIVE_PAUSE;
using rtype::game::testing::CURVE_WITHOUT_BYDOS;
using rtype::game::testing::CURVE_WITHOUT_TIME_BETWEEN_BYDOS;
using rtype::game::testing::MANY_WAVES;
using rtype::game::testing::STEADY_CURVE;
using rtype::game::testing::TEST_FIFTH_WAVE;
using rtype::game::testing::TEST_FOURTH_WAVE;
using rtype::game::testing::TEST_SECOND_WAVE;
using rtype::game::testing::TEST_SIXTH_WAVE;
using rtype::game::testing::TEST_WAVE_CURVE;
using rtype::game::testing::WAVE_AFTER_THE_ONE_JUST_ABOVE;
using rtype::game::testing::WAVE_JUST_ABOVE_THE_SHORTEST;

/**
 * Given the first wave of the test curve, 3 Bydos one every 2 seconds
 * When the wave after it is asked for
 * Then it holds 4 Bydos, one every 1.75 seconds
 */
TEST(WaveProgression, EachWaveHoldsMoreBydosCloserTogether) {
  EXPECT_EQ(bydoWaveAfter(TEST_WAVE_CURVE.firstWave, TEST_WAVE_CURVE),
            TEST_SECOND_WAVE);
}

/**
 * Given the fourth wave of the test curve, its Bydos 1.25 seconds apart
 * When the wave after it is asked for
 * Then its Bydos come one every second, the shortest time of the curve
 */
TEST(WaveProgression, TheFifthWaveReachesTheShortestTime) {
  EXPECT_EQ(bydoWaveAfter(TEST_FOURTH_WAVE, TEST_WAVE_CURVE), TEST_FIFTH_WAVE);
}

/**
 * Given the fifth wave of the test curve, one Bydo every second
 * When the wave after it is asked for
 * Then it holds one more Bydo, still one every second
 */
TEST(WaveProgression, TheWavesAfterTheFifthKeepTheShortestTime) {
  EXPECT_EQ(bydoWaveAfter(TEST_FIFTH_WAVE, TEST_WAVE_CURVE), TEST_SIXTH_WAVE);
}

/**
 * Given a wave whose Bydos come 1.1 seconds apart, less than a cut above the
 * shortest time
 * When the wave after it is asked for
 * Then its Bydos come one every second, not 0.85 seconds apart
 */
TEST(WaveProgression, TheBydosNeverComeCloserThanTheShortestTime) {
  EXPECT_EQ(bydoWaveAfter(WAVE_JUST_ABOVE_THE_SHORTEST, TEST_WAVE_CURVE),
            WAVE_AFTER_THE_ONE_JUST_ABOVE);
}

/**
 * Given a curve that adds no Bydo and cuts nothing
 * When the wave after its first is asked for
 * Then it is the first wave again
 */
TEST(WaveProgression, ACurveThatAddsNothingRepeatsItsFirstWave) {
  EXPECT_EQ(bydoWaveAfter(STEADY_CURVE.firstWave, STEADY_CURVE),
            STEADY_CURVE.firstWave);
}

/**
 * Given the first wave of the test curve
 * When fifty waves follow it
 * Then each holds more Bydos than the one before it
 */
TEST(WaveProgression, EveryWaveHoldsMoreBydosThanTheOneBefore) {
  BydoWave wave = TEST_WAVE_CURVE.firstWave;
  for (std::size_t wavesFollowed = 1; wavesFollowed < MANY_WAVES;
       ++wavesFollowed) {
    const BydoWave next = bydoWaveAfter(wave, TEST_WAVE_CURVE);
    EXPECT_GT(next.bydoCount, wave.bydoCount);
    wave = next;
  }
}

/**
 * Given the first wave of the test curve
 * When fifty waves follow it
 * Then none brings its Bydos closer together than the shortest time
 */
TEST(WaveProgression, NoWaveBringsTheBydosCloserThanTheShortestTime) {
  BydoWave wave = TEST_WAVE_CURVE.firstWave;
  for (std::size_t wavesFollowed = 1; wavesFollowed < MANY_WAVES;
       ++wavesFollowed) {
    wave = bydoWaveAfter(wave, TEST_WAVE_CURVE);
    EXPECT_GE(wave.spawnInterval, TEST_WAVE_CURVE.shortestSpawnInterval);
  }
}

/**
 * Given the test curve, and a curve whose shortest time is its first one
 * When they are checked
 * Then both are playable
 */
TEST(WaveProgression, ACurveWithBydosAndTimeBetweenThemIsPlayable) {
  EXPECT_TRUE(isPlayable(TEST_WAVE_CURVE));
  EXPECT_TRUE(isPlayable(STEADY_CURVE));
}

/**
 * Given the test curve with a first wave of no Bydo
 * When it is checked
 * Then it is not playable
 */
TEST(WaveProgression, ACurveWithoutBydosIsNotPlayable) {
  EXPECT_FALSE(isPlayable(CURVE_WITHOUT_BYDOS));
}

/**
 * Given the test curve with no time as its shortest time between two Bydos
 * When it is checked
 * Then it is not playable
 */
TEST(WaveProgression, ACurveWithoutTimeBetweenBydosIsNotPlayable) {
  EXPECT_FALSE(isPlayable(CURVE_WITHOUT_TIME_BETWEEN_BYDOS));
}

/**
 * Given the test curve with a shortest time longer than its first one
 * When it is checked
 * Then it is not playable: its waves would slow down
 */
TEST(WaveProgression, ACurveThatSlowsDownIsNotPlayable) {
  EXPECT_FALSE(isPlayable(CURVE_THAT_SLOWS_DOWN));
}

/**
 * Given the test curve with a negative cut, and with a negative pause
 * When they are checked
 * Then neither is playable
 */
TEST(WaveProgression, ACurveWithANegativeDurationIsNotPlayable) {
  EXPECT_FALSE(isPlayable(CURVE_WITH_A_NEGATIVE_CUT));
  EXPECT_FALSE(isPlayable(CURVE_WITH_A_NEGATIVE_PAUSE));
}
