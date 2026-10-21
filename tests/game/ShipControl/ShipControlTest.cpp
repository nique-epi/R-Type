#include <gtest/gtest.h>
#include <cmath>
#include <ostream>
#include <string_view>
#include "PlayfieldConstants.hpp"
#include "Position.hpp"
#include "ShipControl.hpp"
#include "ShipControlConstants.hpp"
#include "ShipControlTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::engine::SIMULATION_TICK_DURATION;
using rtype::game::PLAYFIELD_HEIGHT;
using rtype::game::PLAYFIELD_WIDTH;
using rtype::game::Position;
using rtype::game::SHIP_SPEED;
using rtype::game::ShipDirections;
using rtype::game::shipPositionAfter;
using rtype::game::testing::BOTTOM_EDGE_STOP_Y;
using rtype::game::testing::LEFT_EDGE_STOP_X;
using rtype::game::testing::NEAR_AN_EDGE;
using rtype::game::testing::ONE_SECOND;
using rtype::game::testing::PLAYFIELD_CENTER;
using rtype::game::testing::POSITION_TOLERANCE;
using rtype::game::testing::RIGHT_EDGE_STOP_X;
using rtype::game::testing::TOP_EDGE_STOP_Y;

namespace {

/** @brief A direction held alone, named after it, and which way it moves a
 * ship along each axis: -1, 0 or 1. */
struct HeldDirection {
  std::string_view name;
  ShipDirections held;
  float towardsX{};
  float towardsY{};
};

/** @brief Two opposite directions held together, named after their axis. */
struct OppositeDirections {
  std::string_view name;
  ShipDirections held;
};

/** @brief A ship heading for an edge, named after it, and where it must stop:
 * with its side on that edge. */
struct EdgeApproach {
  std::string_view name;
  ShipDirections held;
  Position from{};
  Position stop{};
};

/** @brief Prints a case by its name, so the name of each test says which case
 * it runs. */
void PrintTo(const HeldDirection& direction, std::ostream* output) {
  *output << direction.name;
}

void PrintTo(const OppositeDirections& directions, std::ostream* output) {
  *output << directions.name;
}

void PrintTo(const EdgeApproach& approach, std::ostream* output) {
  *output << approach.name;
}

class ShipControlDirectionTest
    : public ::testing::TestWithParam<HeldDirection> {};

class ShipControlOppositesTest
    : public ::testing::TestWithParam<OppositeDirections> {};

class ShipControlEdgeTest : public ::testing::TestWithParam<EdgeApproach> {};

}  // namespace

/**
 * Given a ship in the middle of the playfield whose player holds one direction
 * When a second passes
 * Then the ship has moved SHIP_SPEED that way, and not along the other axis
 */
TEST_P(ShipControlDirectionTest, MovesAtTheShipSpeedTowardsTheDirectionHeld) {
  const HeldDirection direction = GetParam();

  const Position arrival =
      shipPositionAfter(PLAYFIELD_CENTER, direction.held, ONE_SECOND);

  EXPECT_FLOAT_EQ(arrival.x,
                  PLAYFIELD_CENTER.x + (SHIP_SPEED * direction.towardsX));
  EXPECT_FLOAT_EQ(arrival.y,
                  PLAYFIELD_CENTER.y + (SHIP_SPEED * direction.towardsY));
}

INSTANTIATE_TEST_SUITE_P(EachDirection, ShipControlDirectionTest,
                         ::testing::Values(HeldDirection{.name = "Up",
                                                         .held = {.up = true},
                                                         .towardsX = 0.0F,
                                                         .towardsY = -1.0F},
                                           HeldDirection{.name = "Down",
                                                         .held = {.down = true},
                                                         .towardsX = 0.0F,
                                                         .towardsY = 1.0F},
                                           HeldDirection{.name = "Left",
                                                         .held = {.left = true},
                                                         .towardsX = -1.0F,
                                                         .towardsY = 0.0F},
                                           HeldDirection{
                                               .name = "Right",
                                               .held = {.right = true},
                                               .towardsX = 1.0F,
                                               .towardsY = 0.0F}));

/**
 * Given a ship in the middle of the playfield whose player holds nothing
 * When a second passes
 * Then the ship is where it was
 */
TEST(ShipControl, StaysStillWhenNothingIsHeld) {
  const Position arrival =
      shipPositionAfter(PLAYFIELD_CENTER, ShipDirections{}, ONE_SECOND);

  EXPECT_FLOAT_EQ(arrival.x, PLAYFIELD_CENTER.x);
  EXPECT_FLOAT_EQ(arrival.y, PLAYFIELD_CENTER.y);
}

/**
 * Given a ship in the middle of the playfield whose player holds two opposite
 * directions
 * When a second passes
 * Then the ship is where it was: the two cancel out
 */
TEST_P(ShipControlOppositesTest, StaysStillWhenOppositeDirectionsAreHeld) {
  const Position arrival =
      shipPositionAfter(PLAYFIELD_CENTER, GetParam().held, ONE_SECOND);

  EXPECT_FLOAT_EQ(arrival.x, PLAYFIELD_CENTER.x);
  EXPECT_FLOAT_EQ(arrival.y, PLAYFIELD_CENTER.y);
}

INSTANTIATE_TEST_SUITE_P(
    EachAxis, ShipControlOppositesTest,
    ::testing::Values(OppositeDirections{.name = "UpAndDown",
                                         .held = {.up = true, .down = true}},
                      OppositeDirections{
                          .name = "LeftAndRight",
                          .held = {.left = true, .right = true}}));

/**
 * Given a ship in the middle of the playfield whose player holds up, down and
 * right
 * When a second passes
 * Then the ship has moved SHIP_SPEED to the right: up and down cancel out and
 * leave a straight line, at full speed
 */
TEST(ShipControl, MovesAtFullSpeedWhenOppositesLeaveAStraightLine) {
  const Position arrival = shipPositionAfter(
      PLAYFIELD_CENTER, ShipDirections{.up = true, .down = true, .right = true},
      ONE_SECOND);

  EXPECT_FLOAT_EQ(arrival.x, PLAYFIELD_CENTER.x + SHIP_SPEED);
  EXPECT_FLOAT_EQ(arrival.y, PLAYFIELD_CENTER.y);
}

/**
 * Given a ship in the middle of the playfield whose player holds up and right
 * When a second passes
 * Then the ship has covered SHIP_SPEED, no more, as far towards the right as
 * upwards
 */
TEST(ShipControl, IsNoFasterAlongADiagonalThanAlongAStraightLine) {
  const Position arrival = shipPositionAfter(
      PLAYFIELD_CENTER, ShipDirections{.up = true, .right = true}, ONE_SECOND);

  const float towardsRight = arrival.x - PLAYFIELD_CENTER.x;
  const float upwards = PLAYFIELD_CENTER.y - arrival.y;
  EXPECT_NEAR(std::hypot(towardsRight, upwards), SHIP_SPEED,
              POSITION_TOLERANCE);
  EXPECT_GT(towardsRight, 0.0F);
  EXPECT_NEAR(towardsRight, upwards, POSITION_TOLERANCE);
}

/**
 * Given a ship NEAR_AN_EDGE from an edge whose player holds the direction of
 * that edge
 * When a second passes, long enough to take its center past the edge
 * Then the ship stops with its side on the edge: the whole ship stays on screen
 */
TEST_P(ShipControlEdgeTest, StopsWithItsSideOnTheEdge) {
  const EdgeApproach approach = GetParam();

  const Position arrival =
      shipPositionAfter(approach.from, approach.held, ONE_SECOND);

  EXPECT_FLOAT_EQ(arrival.x, approach.stop.x);
  EXPECT_FLOAT_EQ(arrival.y, approach.stop.y);
}

INSTANTIATE_TEST_SUITE_P(
    EachEdge, ShipControlEdgeTest,
    ::testing::Values(
        EdgeApproach{.name = "RightEdge",
                     .held = {.right = true},
                     .from = {.x = PLAYFIELD_WIDTH - NEAR_AN_EDGE,
                              .y = PLAYFIELD_CENTER.y},
                     .stop = {.x = RIGHT_EDGE_STOP_X, .y = PLAYFIELD_CENTER.y}},
        EdgeApproach{.name = "LeftEdge",
                     .held = {.left = true},
                     .from = {.x = NEAR_AN_EDGE, .y = PLAYFIELD_CENTER.y},
                     .stop = {.x = LEFT_EDGE_STOP_X, .y = PLAYFIELD_CENTER.y}},
        EdgeApproach{.name = "TopEdge",
                     .held = {.up = true},
                     .from = {.x = PLAYFIELD_CENTER.x, .y = NEAR_AN_EDGE},
                     .stop = {.x = PLAYFIELD_CENTER.x, .y = TOP_EDGE_STOP_Y}},
        EdgeApproach{
            .name = "BottomEdge",
            .held = {.down = true},
            .from = {.x = PLAYFIELD_CENTER.x,
                     .y = PLAYFIELD_HEIGHT - NEAR_AN_EDGE},
            .stop = {.x = PLAYFIELD_CENTER.x, .y = BOTTOM_EDGE_STOP_Y}}));

/**
 * Given a ship whose side is on the right edge and whose player holds right
 * When a tick passes
 * Then the ship has not moved: an edge holds a ship pushed against it
 */
TEST(ShipControl, StaysAgainstTheEdgeItIsPushedAgainst) {
  const Position onTheEdge{.x = RIGHT_EDGE_STOP_X, .y = PLAYFIELD_CENTER.y};

  const Position arrival = shipPositionAfter(
      onTheEdge, ShipDirections{.right = true}, SIMULATION_TICK_DURATION);

  EXPECT_FLOAT_EQ(arrival.x, onTheEdge.x);
  EXPECT_FLOAT_EQ(arrival.y, onTheEdge.y);
}
