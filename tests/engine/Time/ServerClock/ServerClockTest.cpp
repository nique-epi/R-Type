#include <gtest/gtest.h>
#include <optional>
#include "ClockSample.hpp"
#include "ServerClock.hpp"
#include "ServerClockFixtures.hpp"
#include "ServerClockTestConstants.hpp"
#include "SymmetricPath.hpp"
#include "TimeConstants.hpp"

using rtype::engine::ClockSample;
using rtype::engine::Duration;
using rtype::engine::ServerClock;
using rtype::engine::testing::addTheBestSampleThenWorseOnes;
using rtype::engine::testing::AN_EIGHTH_OF_FOUR_MILLISECONDS;
using rtype::engine::testing::AN_EIGHTH_OF_TWO_MILLISECONDS;
using rtype::engine::testing::FOUR_MILLISECONDS;
using rtype::engine::testing::HALF_A_SECOND;
using rtype::engine::testing::HALF_A_SECOND_LATER;
using rtype::engine::testing::HALF_THE_DIFFERENCE;
using rtype::engine::testing::LONG_ROUND_TRIP;
using rtype::engine::testing::LONG_WAY;
using rtype::engine::testing::ONE_SECOND_LATER;
using rtype::engine::testing::SAMPLES_BEFORE_THE_BEST_LEAVES;
using rtype::engine::testing::SECOND_GAME_TICK;
using rtype::engine::testing::SECOND_GAME_TICK_30_SENT_AT;
using rtype::engine::testing::SECOND_GAME_TICK_ZERO;
using rtype::engine::testing::SHORT_ROUND_TRIP;
using rtype::engine::testing::SHORT_WAY;
using rtype::engine::testing::SymmetricPath;
using rtype::engine::testing::symmetricSample;
using rtype::engine::testing::TEN_SECONDS_IN;
using rtype::engine::testing::TICK_600_SENT_AT;
using rtype::engine::testing::TICK_630_SENT_AT;
using rtype::engine::testing::TICK_660_SENT_AT;
using rtype::engine::testing::TWO_HUNDRED_MILLISECONDS;
using rtype::engine::testing::TWO_MILLISECONDS;
using rtype::engine::testing::USUAL_ROUND_TRIP;
using rtype::engine::testing::WAYS_OF_A_PATH;

/**
 * Given a clock that received no sample
 * When it is asked for an instant or a tick
 * Then both answers are empty, never a plausible 0
 */
TEST(ServerClock, AnswersNothingBeforeTheFirstSample) {
  const ServerClock clock;

  EXPECT_FALSE(clock.localInstantOf(TEN_SECONDS_IN).has_value());
  EXPECT_FALSE(clock.tickAt(Duration::zero()).has_value());
}

/**
 * Given tick 600 sent while tick 0 stood at instant 0, received 50 ms later
 * on a round trip of 100 ms
 * When the clock is asked at which instant the server was at tick 600
 * Then it answers the instant the server sent it
 */
TEST(ServerClock, PlacesATickWhereTheServerSentItOnASymmetricPath) {
  ServerClock clock;
  clock.add(ClockSample{
      .receivedAt = TICK_600_SENT_AT + USUAL_ROUND_TRIP / WAYS_OF_A_PATH,
      .roundTrip = USUAL_ROUND_TRIP,
      .tick = TEN_SECONDS_IN});

  EXPECT_EQ(clock.localInstantOf(TEN_SECONDS_IN), TICK_600_SENT_AT);
}

/**
 * Given a clock that holds one sample
 * When it is asked the tick at the instant it gave for tick 600
 * Then it answers 600
 */
TEST(ServerClock, GivesBackTheTickItPlacedAtAnInstant) {
  ServerClock clock;
  clock.add(symmetricSample(TEN_SECONDS_IN,
                            SymmetricPath{.tickZero = Duration::zero(),
                                          .roundTrip = USUAL_ROUND_TRIP}));

  EXPECT_DOUBLE_EQ(clock.tickAt(TICK_600_SENT_AT).value_or(-1.0),
                   static_cast<double>(TEN_SECONDS_IN));
}

/**
 * Given a heartbeat that went out in 10 ms and came back in 50 ms
 * When the clock places tick 600 from it
 * Then it is early by half the difference of the two ways, 20 ms: the
 * estimate cannot tell the ways apart
 */
TEST(ServerClock, ErrsByHalfTheDifferenceOfTheTwoWaysOfAnAsymmetricPath) {
  ServerClock clock;
  clock.add(ClockSample{.receivedAt = TICK_600_SENT_AT + SHORT_WAY,
                        .roundTrip = SHORT_WAY + LONG_WAY,
                        .tick = TEN_SECONDS_IN});

  EXPECT_EQ(clock.localInstantOf(TEN_SECONDS_IN),
            TICK_600_SENT_AT - HALF_THE_DIFFERENCE);
}

/**
 * Given a clock that read a heartbeat on a round trip of 100 ms, then one that
 * waited in a queue (a round trip of 500 ms, so 200 ms late on the estimate)
 * When it is asked at which instant the server was at tick 630
 * Then it answers where the first, shorter one puts it
 */
TEST(ServerClock, LeavesAsideASampleWhoseRoundTripIsLong) {
  ServerClock clock;
  clock.add(symmetricSample(TEN_SECONDS_IN,
                            SymmetricPath{.tickZero = Duration::zero(),
                                          .roundTrip = USUAL_ROUND_TRIP}));
  const ClockSample delayed = symmetricSample(
      HALF_A_SECOND_LATER, SymmetricPath{.tickZero = TWO_HUNDRED_MILLISECONDS,
                                         .roundTrip = LONG_ROUND_TRIP});

  clock.add(delayed);

  EXPECT_EQ(clock.localInstantOf(HALF_A_SECOND_LATER), TICK_630_SENT_AT);
}

/**
 * Given a clock at instant 0 for tick 0
 * When a sample with a shorter round trip says tick 0 stood 2 ms later
 * Then the clock moves by an eighth of the gap, 250 µs, not all of it
 */
TEST(ServerClock, ClosesAnEighthOfTheGapToABetterSample) {
  ServerClock clock;
  clock.add(symmetricSample(TEN_SECONDS_IN,
                            SymmetricPath{.tickZero = Duration::zero(),
                                          .roundTrip = USUAL_ROUND_TRIP}));

  clock.add(symmetricSample(ONE_SECOND_LATER,
                            SymmetricPath{.tickZero = TWO_MILLISECONDS,
                                          .roundTrip = SHORT_ROUND_TRIP}));

  EXPECT_EQ(clock.localInstantOf(ONE_SECOND_LATER),
            TICK_660_SENT_AT + AN_EIGHTH_OF_TWO_MILLISECONDS);
}

/**
 * Given a clock at instant 0 for tick 0
 * When a sample with the same round trip says tick 0 stood 2 ms later
 * Then the clock follows the newest of the two, and moves by an eighth of the
 * gap, 250 µs
 */
TEST(ServerClock, FollowsTheNewestOfTwoSamplesWithTheSameRoundTrip) {
  ServerClock clock;
  clock.add(symmetricSample(TEN_SECONDS_IN,
                            SymmetricPath{.tickZero = Duration::zero(),
                                          .roundTrip = USUAL_ROUND_TRIP}));

  clock.add(symmetricSample(ONE_SECOND_LATER,
                            SymmetricPath{.tickZero = TWO_MILLISECONDS,
                                          .roundTrip = USUAL_ROUND_TRIP}));

  EXPECT_EQ(clock.localInstantOf(ONE_SECOND_LATER),
            TICK_660_SENT_AT + AN_EIGHTH_OF_TWO_MILLISECONDS);
}

/**
 * Given a clock at instant 0 for tick 0
 * When a sample with a shorter round trip says tick 0 stood half a second
 * later, more than six ticks
 * Then the clock jumps onto it at once
 */
TEST(ServerClock, JumpsOntoASampleFarFromWhereItWas) {
  ServerClock clock;
  clock.add(symmetricSample(TEN_SECONDS_IN,
                            SymmetricPath{.tickZero = Duration::zero(),
                                          .roundTrip = USUAL_ROUND_TRIP}));

  clock.add(symmetricSample(
      ONE_SECOND_LATER,
      SymmetricPath{.tickZero = HALF_A_SECOND, .roundTrip = SHORT_ROUND_TRIP}));

  EXPECT_EQ(clock.localInstantOf(ONE_SECOND_LATER),
            TICK_660_SENT_AT + HALF_A_SECOND);
}

/**
 * Given a clock that read a game up to tick 600 on a round trip of 60 ms
 * When a sample of tick 30 arrives, on a round trip of 100 ms, for a server
 * whose tick 0 stood at 100 s
 * Then the first game is forgotten: the clock places tick 30 where the new
 * game sent it
 */
TEST(ServerClock, ForgetsTheGameBeforeWhenTheTickGoesBack) {
  ServerClock clock;
  clock.add(symmetricSample(TEN_SECONDS_IN,
                            SymmetricPath{.tickZero = Duration::zero(),
                                          .roundTrip = SHORT_ROUND_TRIP}));

  clock.add(symmetricSample(SECOND_GAME_TICK,
                            SymmetricPath{.tickZero = SECOND_GAME_TICK_ZERO,
                                          .roundTrip = USUAL_ROUND_TRIP}));

  EXPECT_EQ(clock.localInstantOf(SECOND_GAME_TICK),
            SECOND_GAME_TICK_30_SENT_AT);
}

/**
 * Given a clock that holds samples
 * When it is reset
 * Then it answers nothing again and holds no sample
 */
TEST(ServerClock, AnswersNothingAfterAReset) {
  ServerClock clock;
  clock.add(symmetricSample(TEN_SECONDS_IN,
                            SymmetricPath{.tickZero = Duration::zero(),
                                          .roundTrip = USUAL_ROUND_TRIP}));

  clock.reset();

  EXPECT_FALSE(clock.localInstantOf(TEN_SECONDS_IN).has_value());
  EXPECT_EQ(clock.sampleCount(), 0U);
}

/**
 * Given a clock that read a sample with the best round trip, 20 ms, then
 * seven with 100 ms
 * When it is asked for tick 630
 * Then it still follows the best sample, which has not left the window
 */
TEST(ServerClock, KeepsFollowingTheBestSampleWhileItIsInTheWindow) {
  ServerClock clock;

  addTheBestSampleThenWorseOnes(clock);

  EXPECT_EQ(clock.localInstantOf(TEN_SECONDS_IN), TICK_600_SENT_AT);
}

/**
 * Given the same clock, with the best sample now the oldest of eight
 * When one more sample arrives
 * Then the best one leaves the window and the clock closes an eighth of the
 * gap, 500 µs, to the 4 ms the others say
 */
TEST(ServerClock, ForgetsTheBestSampleOnceTheWindowIsFull) {
  ServerClock clock;
  addTheBestSampleThenWorseOnes(clock);

  clock.add(symmetricSample(TEN_SECONDS_IN + SAMPLES_BEFORE_THE_BEST_LEAVES + 1,
                            SymmetricPath{.tickZero = FOUR_MILLISECONDS,
                                          .roundTrip = USUAL_ROUND_TRIP}));

  EXPECT_EQ(clock.localInstantOf(TEN_SECONDS_IN),
            TICK_600_SENT_AT + AN_EIGHTH_OF_FOUR_MILLISECONDS);
}
