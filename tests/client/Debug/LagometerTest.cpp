#include <gtest/gtest.h>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>
#include "Lagometer.hpp"
#include "LagometerFixtures.hpp"
#include "LagometerPoint.hpp"
#include "LagometerTestConstants.hpp"
#include "RoundTripSample.hpp"
#include "SessionArrivals.hpp"

namespace rtype::client {

using testing::ACCEPTED_SINCE_BEFORE;
using testing::ARRIVALS_AT_FIRST_SAMPLE;
using testing::ARRIVALS_BEFORE;
using testing::AT_THE_LOWEST_TOP;
using testing::BYTES_AFTER_A_SECOND;
using testing::BYTES_BEFORE;
using testing::BYTES_IN_THAT_SECOND;
using testing::DOUBLED_TOP;
using testing::FIFTY_FRAMES_PER_SECOND;
using testing::FIRST_SAMPLE;
using testing::frameReading;
using testing::FRAMES_IN_A_SECOND;
using testing::JUST_ABOVE_THE_LOWEST_TOP;
using testing::MISSED_SINCE_BEFORE;
using testing::OLDEST_KEPT_ROUND_TRIP;
using testing::ONE_PAST_THE_CURVE;
using testing::sampleReading;
using testing::TEN_PERCENT_LOST;
using testing::trafficReading;
using testing::TWENTY_MILLISECONDS;

/**
 * Given a lagometer
 * When a frame reads the first round trip, 120 ms
 * Then the curve holds one point of 120 ms
 */
TEST(Lagometer, AFirstRoundTripBecomesAPoint) {
  Lagometer lagometer;

  lagometer.update(sampleReading(FIRST_SAMPLE, SessionArrivals{}));

  ASSERT_EQ(lagometer.points().size(), 1U);
  EXPECT_EQ(lagometer.points().front().roundTrip, FIRST_SAMPLE.roundTrip);
}

/**
 * Given a lagometer that took the first round trip
 * When the next frame reads the same round trip again
 * Then the curve still holds one point
 */
TEST(Lagometer, TheSameRoundTripReadTwiceIsOnePoint) {
  Lagometer lagometer;
  lagometer.update(sampleReading(FIRST_SAMPLE, SessionArrivals{}));

  lagometer.update(sampleReading(FIRST_SAMPLE, SessionArrivals{}));

  EXPECT_EQ(lagometer.points().size(), 1U);
}

/**
 * Given a lagometer whose first frame read 10 datagrams accepted and no round
 * trip
 * When the first round trip comes with 28 accepted and 2 missed
 * Then its point counts the 18 accepted and the 2 missed since, and the
 * panel reads 10 % lost
 */
TEST(Lagometer, APointCountsTheDatagramsSinceTheFirstFrame) {
  Lagometer lagometer;
  lagometer.update(sampleReading(RoundTripSample{}, ARRIVALS_BEFORE));

  lagometer.update(sampleReading(FIRST_SAMPLE, ARRIVALS_AT_FIRST_SAMPLE));

  EXPECT_EQ(lagometer.points().back(),
            (LagometerPoint{.roundTrip = FIRST_SAMPLE.roundTrip,
                            .accepted = ACCEPTED_SINCE_BEFORE,
                            .missed = MISSED_SINCE_BEFORE}));
  EXPECT_EQ(lagometer.figures().lossPercent, TEN_PERCENT_LOST);
}

/**
 * Given a lagometer
 * When 61 round trips come, one per frame
 * Then the curve keeps the newest 60, from the second one
 */
TEST(Lagometer, KeepsTheNewestRoundTripsOfTheCurve) {
  Lagometer lagometer;

  for (std::size_t number = 1; number <= ONE_PAST_THE_CURVE; ++number) {
    lagometer.update(sampleReading(
        RoundTripSample{
            .number = number,
            .roundTrip =
                std::chrono::milliseconds{
                    static_cast<std::chrono::milliseconds::rep>(number)}},
        SessionArrivals{}));
  }

  ASSERT_EQ(lagometer.points().size(), ONE_PAST_THE_CURVE - 1);
  EXPECT_EQ(lagometer.points().front().roundTrip, OLDEST_KEPT_ROUND_TRIP);
}

/**
 * Given a lagometer with no point
 * When the top of its scale is read
 * Then it is the lowest top, 50 ms
 */
TEST(Lagometer, TheScaleStartsAtItsLowestTop) {
  const Lagometer lagometer;

  EXPECT_EQ(lagometer.scaleTop(), AT_THE_LOWEST_TOP);
}

/**
 * Given a lagometer whose highest round trip is exactly 50 ms
 * When the top of its scale is read
 * Then it stays 50 ms: the point lies on the top
 */
TEST(Lagometer, ARoundTripOnTheTopKeepsTheTop) {
  Lagometer lagometer;

  lagometer.update(sampleReading(
      RoundTripSample{.number = 1, .roundTrip = AT_THE_LOWEST_TOP},
      SessionArrivals{}));

  EXPECT_EQ(lagometer.scaleTop(), AT_THE_LOWEST_TOP);
}

/**
 * Given a lagometer whose highest round trip is 1 µs above 50 ms
 * When the top of its scale is read
 * Then it doubles, to 100 ms
 */
TEST(Lagometer, ARoundTripAboveTheTopDoublesTheTop) {
  Lagometer lagometer;

  lagometer.update(sampleReading(
      RoundTripSample{.number = 1, .roundTrip = JUST_ABOVE_THE_LOWEST_TOP},
      SessionArrivals{}));

  EXPECT_EQ(lagometer.scaleTop(), DOUBLED_TOP);
}

/**
 * Given a lagometer
 * When 49 frames of 20 ms pass, then one more
 * Then no frame rate is known before the second ends, then 50 frames per
 * second
 */
TEST(Lagometer, CountsTheFramesOfEachSecond) {
  Lagometer lagometer;
  for (std::size_t frame = 1; frame < FRAMES_IN_A_SECOND; ++frame) {
    lagometer.update(frameReading(TWENTY_MILLISECONDS));
  }
  const std::optional<std::uint64_t> beforeTheSecond =
      lagometer.figures().framesPerSecond;

  lagometer.update(frameReading(TWENTY_MILLISECONDS));

  EXPECT_FALSE(beforeTheSecond.has_value());
  EXPECT_EQ(lagometer.figures().framesPerSecond.value_or(0),
            FIFTY_FRAMES_PER_SECOND);
}

/**
 * Given a lagometer whose first frame read 1000 bytes received
 * When a frame of a second ends with 13000 bytes received
 * Then 12000 bytes were received in that second
 */
TEST(Lagometer, CountsTheBytesReceivedSinceTheFirstFrame) {
  Lagometer lagometer;
  lagometer.update(
      trafficReading(std::chrono::milliseconds::zero(), BYTES_BEFORE));

  lagometer.update(
      trafficReading(std::chrono::seconds{1}, BYTES_AFTER_A_SECOND));

  EXPECT_EQ(lagometer.figures().bytesReceivedPerSecond.value_or(0),
            BYTES_IN_THAT_SECOND);
}

}  // namespace rtype::client
