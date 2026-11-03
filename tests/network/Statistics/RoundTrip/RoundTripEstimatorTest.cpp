#include <gtest/gtest.h>
#include <chrono>
#include <cstddef>
#include <optional>
#include "RoundTripEstimator.hpp"
#include "RoundTripEstimatorTestConstants.hpp"
#include "RoundTripFigures.hpp"

namespace rtype::network {

using testing::CALM_ROUND_TRIP;
using testing::FIRST_ROUND_TRIP;
using testing::HIGH_ROUND_TRIP;
using testing::JITTER_OF_ONE_SPIKE_IN_THE_WINDOW;
using testing::JITTER_OF_THE_THREE;
using testing::LOW_ROUND_TRIP;
using testing::MIDDLE_ROUND_TRIP;
using testing::ROUND_TRIPS_IN_THE_WINDOW;
using testing::SECOND_ROUND_TRIP;
using testing::SMOOTHED_AFTER_THE_SECOND;
using testing::SPIKE_ROUND_TRIP;

/**
 * Given an estimator given no round trip
 * When its figures are asked
 * Then there are none
 */
TEST(RoundTripEstimator, GivesNothingBeforeTheFirstRoundTrip) {
  const RoundTripEstimator estimator;

  EXPECT_FALSE(estimator.figures().has_value());
}

/**
 * Given an estimator
 * When it is given one round trip of 40 ms
 * Then the smoothed round trip is 40 ms and the jitter 0
 */
TEST(RoundTripEstimator, TakesTheFirstRoundTripAsItIs) {
  RoundTripEstimator estimator;

  estimator.add(FIRST_ROUND_TRIP);

  const std::optional<RoundTripFigures> figures = estimator.figures();
  ASSERT_TRUE(figures.has_value());
  EXPECT_EQ(figures.value_or(RoundTripFigures{}),
            (RoundTripFigures{.roundTrip = FIRST_ROUND_TRIP,
                              .jitter = std::chrono::microseconds::zero()}));
}

/**
 * Given an estimator given a round trip of 40 ms
 * When it is given one of 80 ms
 * Then the smoothed round trip moves an eighth of the gap, to 45 ms, and the
 * jitter is the gap, 40 ms
 */
TEST(RoundTripEstimator, MovesAnEighthOfTheGapTowardEachRoundTrip) {
  RoundTripEstimator estimator;
  estimator.add(FIRST_ROUND_TRIP);

  estimator.add(SECOND_ROUND_TRIP);

  const std::optional<RoundTripFigures> figures = estimator.figures();
  ASSERT_TRUE(figures.has_value());
  EXPECT_EQ(figures.value_or(RoundTripFigures{}),
            (RoundTripFigures{.roundTrip = SMOOTHED_AFTER_THE_SECOND,
                              .jitter = SECOND_ROUND_TRIP - FIRST_ROUND_TRIP}));
}

/**
 * Given an estimator
 * When it is given round trips of 10, 30 and 20 ms
 * Then the jitter is the mean of the gaps of 20 and 10 ms, 15 ms
 */
TEST(RoundTripEstimator, AveragesTheGapsBetweenRoundTripsInARow) {
  RoundTripEstimator estimator;

  estimator.add(LOW_ROUND_TRIP);
  estimator.add(HIGH_ROUND_TRIP);
  estimator.add(MIDDLE_ROUND_TRIP);

  const std::optional<RoundTripFigures> figures = estimator.figures();
  ASSERT_TRUE(figures.has_value());
  EXPECT_EQ(figures.value_or(RoundTripFigures{}).jitter, JITTER_OF_THE_THREE);
}

/**
 * Given an estimator
 * When it is given a spike of 100 ms, then 15 round trips of 40 ms
 * Then the spike is still among the 16 round trips of the jitter: one gap of
 * 60 ms in 15 makes 4 ms
 */
TEST(RoundTripEstimator, KeepsSixteenRoundTripsForTheJitter) {
  RoundTripEstimator estimator;

  estimator.add(SPIKE_ROUND_TRIP);
  for (std::size_t added = 1; added < ROUND_TRIPS_IN_THE_WINDOW; ++added) {
    estimator.add(CALM_ROUND_TRIP);
  }

  const std::optional<RoundTripFigures> figures = estimator.figures();
  ASSERT_TRUE(figures.has_value());
  EXPECT_EQ(figures.value_or(RoundTripFigures{}).jitter,
            JITTER_OF_ONE_SPIKE_IN_THE_WINDOW);
}

/**
 * Given an estimator
 * When it is given a spike of 100 ms, then 16 round trips of 40 ms
 * Then the spike left the 16 newest round trips: the jitter is 0
 */
TEST(RoundTripEstimator, DropsTheOldestRoundTripPastSixteen) {
  RoundTripEstimator estimator;

  estimator.add(SPIKE_ROUND_TRIP);
  for (std::size_t added = 0; added < ROUND_TRIPS_IN_THE_WINDOW; ++added) {
    estimator.add(CALM_ROUND_TRIP);
  }

  const std::optional<RoundTripFigures> figures = estimator.figures();
  ASSERT_TRUE(figures.has_value());
  EXPECT_EQ(figures.value_or(RoundTripFigures{}).jitter,
            std::chrono::microseconds::zero());
}

/**
 * Given an estimator given 16 round trips of 40 ms, a full window
 * When it is given a spike of 100 ms, which takes the place of the oldest
 * Then the jitter counts the spike as the newest: one gap of 60 ms in 15
 */
TEST(RoundTripEstimator, KeepsTheNewestRoundTripWhenTheWindowWraps) {
  RoundTripEstimator estimator;
  for (std::size_t added = 0; added < ROUND_TRIPS_IN_THE_WINDOW; ++added) {
    estimator.add(CALM_ROUND_TRIP);
  }

  estimator.add(SPIKE_ROUND_TRIP);

  const std::optional<RoundTripFigures> figures = estimator.figures();
  ASSERT_TRUE(figures.has_value());
  EXPECT_EQ(figures.value_or(RoundTripFigures{}).jitter,
            JITTER_OF_ONE_SPIKE_IN_THE_WINDOW);
}

}  // namespace rtype::network
