#include <gtest/gtest.h>
#include <SFML/Graphics/VertexArray.hpp>
#include "Lagometer.hpp"
#include "LagometerConstants.hpp"
#include "LagometerFixtures.hpp"
#include "LagometerPainting.hpp"
#include "LagometerTestConstants.hpp"
#include "RoundTripSample.hpp"
#include "SessionArrivals.hpp"

namespace rtype::client {

using testing::AT_THE_TOP;
using testing::CURVE_BOTTOM;
using testing::CURVE_LEFT;
using testing::CURVE_MIDDLE;
using testing::CURVE_RIGHT;
using testing::CURVE_STEP;
using testing::CURVE_TOP;
using testing::HALF_THE_TOP;
using testing::ONE_MISSED;
using testing::sampleReading;
using testing::SCALE_LINE_VERTICES;

/**
 * Given a lagometer with no point, drawn at a scale of 1
 * When its curve is built
 * Then it holds the three lines of the scale, from the left of the curve to
 * its right, at the bottom, the middle and the top
 */
TEST(LagometerPainting, TheScaleLinesCrossTheWholeCurve) {
  const Lagometer lagometer;

  const sf::VertexArray lines = lagometerCurve(lagometer, 1);

  ASSERT_EQ(lines.getVertexCount(), SCALE_LINE_VERTICES);
  EXPECT_FLOAT_EQ(lines[0].position.x, CURVE_LEFT);
  EXPECT_FLOAT_EQ(lines[1].position.x, CURVE_RIGHT);
  EXPECT_FLOAT_EQ(lines[0].position.y, CURVE_BOTTOM);
  EXPECT_FLOAT_EQ(lines[2].position.y, CURVE_MIDDLE);
  EXPECT_FLOAT_EQ(lines[4].position.y, CURVE_TOP);
}

/**
 * Given a lagometer whose round trips were 25 ms then 50 ms, a top of 50 ms
 * When its curve is built at a scale of 1
 * Then the line goes from half the height, one step left of the right edge,
 * to the top, on the right edge
 */
TEST(LagometerPainting, ALineJoinsEachRoundTripToTheNext) {
  Lagometer lagometer;
  lagometer.update(sampleReading(HALF_THE_TOP, SessionArrivals{}));
  lagometer.update(sampleReading(AT_THE_TOP, SessionArrivals{}));

  const sf::VertexArray lines = lagometerCurve(lagometer, 1);

  ASSERT_EQ(lines.getVertexCount(), SCALE_LINE_VERTICES + 2);
  EXPECT_FLOAT_EQ(lines[SCALE_LINE_VERTICES].position.x,
                  CURVE_RIGHT - CURVE_STEP);
  EXPECT_FLOAT_EQ(lines[SCALE_LINE_VERTICES].position.y, CURVE_MIDDLE);
  EXPECT_FLOAT_EQ(lines[SCALE_LINE_VERTICES + 1].position.x, CURVE_RIGHT);
  EXPECT_FLOAT_EQ(lines[SCALE_LINE_VERTICES + 1].position.y, CURVE_TOP);
}

/**
 * Given a lagometer whose only round trip came with one datagram missed since
 * its first frame
 * When its curve is built at a scale of 1
 * Then a red line crosses the curve at that point, from its top to its bottom
 */
TEST(LagometerPainting, ALostDatagramIsARedLineAcrossTheCurve) {
  Lagometer lagometer;
  lagometer.update(sampleReading(RoundTripSample{}, SessionArrivals{}));
  lagometer.update(sampleReading(HALF_THE_TOP, ONE_MISSED));

  const sf::VertexArray lines = lagometerCurve(lagometer, 1);

  ASSERT_EQ(lines.getVertexCount(), SCALE_LINE_VERTICES + 2);
  EXPECT_EQ(lines[SCALE_LINE_VERTICES].color, LOSS_RED);
  EXPECT_FLOAT_EQ(lines[SCALE_LINE_VERTICES].position.x, CURVE_RIGHT);
  EXPECT_FLOAT_EQ(lines[SCALE_LINE_VERTICES].position.y, CURVE_TOP);
  EXPECT_FLOAT_EQ(lines[SCALE_LINE_VERTICES + 1].position.y, CURVE_BOTTOM);
}

}  // namespace rtype::client
