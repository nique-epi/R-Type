#include <gtest/gtest.h>
#include <vector>
#include "Bounds.hpp"
#include "Collision.hpp"
#include "CollisionTestConstants.hpp"
#include "EventBus.hpp"
#include "EventBusTestConstants.hpp"
#include "Overlaps.hpp"

using rtype::engine::Bounds;
using rtype::engine::Collision;
using rtype::engine::EventBus;
using rtype::engine::overlaps;

namespace {

Bounds squareAt(float centerX, float centerY, float side = SQUARE_SIDE) {
  return Bounds{
      .centerX = centerX, .centerY = centerY, .width = side, .height = side};
}

}  // namespace

/**
 * Given two squares whose centers are closer than their side on the x axis
 * When their overlap is tested
 * Then they overlap
 */
TEST(Overlaps, SquaresSharingAreaOnTheHorizontalAxisOverlap) {
  const Bounds first = squareAt(0.0F, 0.0F);
  const Bounds second = squareAt(CENTER_DISTANCE_OVERLAPPING, 0.0F);

  EXPECT_TRUE(overlaps(first, second));
}

/**
 * Given two squares whose centers are closer than their side on the y axis
 * When their overlap is tested
 * Then they overlap
 */
TEST(Overlaps, SquaresSharingAreaOnTheVerticalAxisOverlap) {
  const Bounds first = squareAt(0.0F, 0.0F);
  const Bounds second = squareAt(0.0F, CENTER_DISTANCE_OVERLAPPING);

  EXPECT_TRUE(overlaps(first, second));
}

/**
 * Given two squares with the same center and the same size
 * When their overlap is tested
 * Then they overlap
 */
TEST(Overlaps, IdenticalSquaresOverlap) {
  const Bounds first = squareAt(0.0F, 0.0F);

  EXPECT_TRUE(overlaps(first, first));
}

/**
 * Given a small square strictly inside a larger one
 * When their overlap is tested
 * Then they overlap
 */
TEST(Overlaps, SquareInsideAnotherOverlaps) {
  const Bounds outer = squareAt(0.0F, 0.0F);
  const Bounds inner = squareAt(0.0F, 0.0F, SMALL_SQUARE_SIDE);

  EXPECT_TRUE(overlaps(outer, inner));
}

/**
 * Given two squares with a gap between them on the x axis
 * When their overlap is tested
 * Then they do not overlap
 */
TEST(Overlaps, SquaresApartOnTheHorizontalAxisDoNotOverlap) {
  const Bounds first = squareAt(0.0F, 0.0F);
  const Bounds second = squareAt(CENTER_DISTANCE_APART, 0.0F);

  EXPECT_FALSE(overlaps(first, second));
}

/**
 * Given two squares with a gap between them on the y axis
 * When their overlap is tested
 * Then they do not overlap
 */
TEST(Overlaps, SquaresApartOnTheVerticalAxisDoNotOverlap) {
  const Bounds first = squareAt(0.0F, 0.0F);
  const Bounds second = squareAt(0.0F, CENTER_DISTANCE_APART);

  EXPECT_FALSE(overlaps(first, second));
}

/**
 * Given two squares touching along a vertical edge
 * When their overlap is tested
 * Then they do not overlap, because they share no area
 */
TEST(Overlaps, SquaresTouchingAlongAVerticalEdgeDoNotOverlap) {
  const Bounds first = squareAt(0.0F, 0.0F);
  const Bounds second = squareAt(CENTER_DISTANCE_TOUCHING, 0.0F);

  EXPECT_FALSE(overlaps(first, second));
}

/**
 * Given two squares touching along a horizontal edge
 * When their overlap is tested
 * Then they do not overlap, because they share no area
 */
TEST(Overlaps, SquaresTouchingAlongAHorizontalEdgeDoNotOverlap) {
  const Bounds first = squareAt(0.0F, 0.0F);
  const Bounds second = squareAt(0.0F, CENTER_DISTANCE_TOUCHING);

  EXPECT_FALSE(overlaps(first, second));
}

/**
 * Given two squares touching only at a corner
 * When their overlap is tested
 * Then they do not overlap, because they share no area
 */
TEST(Overlaps, SquaresTouchingAtACornerDoNotOverlap) {
  const Bounds first = squareAt(0.0F, 0.0F);
  const Bounds second =
      squareAt(CENTER_DISTANCE_TOUCHING, CENTER_DISTANCE_TOUCHING);

  EXPECT_FALSE(overlaps(first, second));
}

/**
 * Given a rectangle with no area strictly inside a square
 * When their overlap is tested
 * Then they overlap, because the point lies in the interior of the square
 */
TEST(Overlaps, RectangleWithoutAreaInsideASquareOverlaps) {
  const Bounds square = squareAt(0.0F, 0.0F);
  const Bounds point = squareAt(0.0F, 0.0F, EMPTY_SIDE);

  EXPECT_TRUE(overlaps(square, point));
}

/**
 * Given a rectangle with no area lying on the edge of a square
 * When their overlap is tested
 * Then they do not overlap, because the point is not in the interior
 */
TEST(Overlaps, RectangleWithoutAreaOnTheEdgeOfASquareDoesNotOverlap) {
  const Bounds square = squareAt(0.0F, 0.0F);
  const Bounds point = squareAt(EDGE_OFFSET, 0.0F, EMPTY_SIDE);

  EXPECT_FALSE(overlaps(square, point));
}

/**
 * Given an overlapping pair and a pair that only touches
 * When each pair is tested in both orders
 * Then both orders give the same answer
 */
TEST(Overlaps, AnswerDoesNotDependOnTheOrderOfTheArguments) {
  const Bounds reference = squareAt(0.0F, 0.0F);
  const Bounds overlappingSquare = squareAt(CENTER_DISTANCE_OVERLAPPING, 0.0F);
  const Bounds touchingSquare = squareAt(CENTER_DISTANCE_TOUCHING, 0.0F);

  EXPECT_EQ(overlaps(reference, overlappingSquare),
            overlaps(overlappingSquare, reference));
  EXPECT_EQ(overlaps(reference, touchingSquare),
            overlaps(touchingSquare, reference));
}

/**
 * Given two squares known at compile time
 * When their overlap is tested in a constant expression
 * Then the compiler evaluates it
 */
TEST(Overlaps, IsUsableInAConstantExpression) {
  constexpr Bounds first{.centerX = 0.0F,
                         .centerY = 0.0F,
                         .width = SQUARE_SIDE,
                         .height = SQUARE_SIDE};
  constexpr Bounds second{.centerX = CENTER_DISTANCE_OVERLAPPING,
                          .centerY = 0.0F,
                          .width = SQUARE_SIDE,
                          .height = SQUARE_SIDE};

  static_assert(overlaps(first, second));
  SUCCEED();
}

/**
 * Given a subscriber to the engine Collision event
 * When a Collision is published, then dispatched
 * Then the subscriber received that pair of entities once
 */
TEST(Collision, EventBusDeliversThePublishedPair) {
  EventBus events;
  std::vector<Collision> received;
  events.subscribe<Collision>([&received](const Collision& collision) {
    received.push_back(collision);
  });

  events.publish(Collision{.first = FIRST_ENTITY, .second = SECOND_ENTITY});
  events.dispatch();

  EXPECT_EQ(received, (std::vector<Collision>{Collision{
                          .first = FIRST_ENTITY, .second = SECOND_ENTITY}}));
}
