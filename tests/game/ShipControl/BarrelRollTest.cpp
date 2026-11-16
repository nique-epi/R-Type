#include <gtest/gtest.h>
#include <optional>
#include "BarrelRoll.hpp"
#include "BarrelRollTestConstants.hpp"
#include "ManeuverKind.hpp"
#include "RollMoment.hpp"
#include "RolledRows.hpp"
#include "ShipMotion.hpp"

using rtype::game::ManeuverKind;
using rtype::game::RolledRows;
using rtype::game::rolledRowsOf;
using rtype::game::RollMoment;
using rtype::game::rollShareOf;
using rtype::game::ShipMotion;
using rtype::game::sourceRowOf;
using rtype::game::testing::END_OF_A_ROLL;
using rtype::game::testing::FIRST_ROW_EDGE_ON;
using rtype::game::testing::FIRST_ROW_EDGE_ON_BELLY;
using rtype::game::testing::HALF_OF_THE_ROLL;
using rtype::game::testing::HALF_TURN;
using rtype::game::testing::HALFWAY_THROUGH_A_ROLL;
using rtype::game::testing::LAST_ROW_OF_THE_FRAME;
using rtype::game::testing::PAST_THE_END_OF_A_ROLL;
using rtype::game::testing::QUARTER_TURN;
using rtype::game::testing::SECOND_ROW_EDGE_ON;
using rtype::game::testing::SECOND_ROW_EDGE_ON_BELLY;
using rtype::game::testing::SHIP_FRAME_ROWS;
using rtype::game::testing::START_OF_A_ROLL;
using rtype::game::testing::THINNEST_ROWS_SEEN;
using rtype::game::testing::WHOLE_ROLL;

/**
 * Given a frame of 16 rows at the start of a roll
 * When the rows it shows are taken
 * Then every row shows, on its back
 */
TEST(BarrelRoll, AShipStartingItsRollShowsEveryRowOnItsBack) {
  const RolledRows rolled = rolledRowsOf(
      RollMoment{.frameHeight = SHIP_FRAME_ROWS, .share = START_OF_A_ROLL});

  EXPECT_EQ(rolled.count, SHIP_FRAME_ROWS);
  EXPECT_FALSE(rolled.belly);
}

/**
 * Given a frame of 16 rows a quarter turn into a roll, seen edge on
 * When the rows it shows are taken
 * Then it keeps the 2 rows a ship keeps at least
 */
TEST(BarrelRoll, AShipSeenEdgeOnKeepsItsThinnestRows) {
  EXPECT_EQ(rolledRowsOf(RollMoment{.frameHeight = SHIP_FRAME_ROWS,
                                    .share = QUARTER_TURN})
                .count,
            THINNEST_ROWS_SEEN);
}

/**
 * Given a frame of 16 rows half a turn into a roll
 * When the rows it shows are taken
 * Then every row shows, on its belly
 */
TEST(BarrelRoll, AShipHalfwayThroughItsRollShowsItsBelly) {
  const RolledRows rolled = rolledRowsOf(
      RollMoment{.frameHeight = SHIP_FRAME_ROWS, .share = HALF_TURN});

  EXPECT_EQ(rolled.count, SHIP_FRAME_ROWS);
  EXPECT_TRUE(rolled.belly);
}

/**
 * Given the 2 rows of a frame of 16 rows seen edge on, on its back
 * When the rows of the frame they show are taken
 * Then they show rows 4 and 12, the middle of each half
 */
TEST(BarrelRoll, TheRowsSeenEdgeOnAreTheMiddleOfEachHalf) {
  const RolledRows rolled{
      .height = SHIP_FRAME_ROWS, .count = THINNEST_ROWS_SEEN, .belly = false};

  EXPECT_EQ(sourceRowOf(rolled, 0), FIRST_ROW_EDGE_ON);
  EXPECT_EQ(sourceRowOf(rolled, 1), SECOND_ROW_EDGE_ON);
}

/**
 * Given the 2 rows of a frame of 16 rows seen edge on, on its belly
 * When the rows of the frame they show are taken
 * Then they show the rows that mirror them, 11 and 3: the ship upside down
 */
TEST(BarrelRoll, TheRowsOfTheBellyAreUpsideDown) {
  const RolledRows rolled{
      .height = SHIP_FRAME_ROWS, .count = THINNEST_ROWS_SEEN, .belly = true};

  EXPECT_EQ(sourceRowOf(rolled, 0), FIRST_ROW_EDGE_ON_BELLY);
  EXPECT_EQ(sourceRowOf(rolled, 1), SECOND_ROW_EDGE_ON_BELLY);
}

/**
 * Given every row of a frame of 16 rows, on its back then on its belly
 * When the rows of the frame its first and its last show are taken
 * Then the back shows rows 0 and 15, the belly rows 15 and 0
 */
TEST(BarrelRoll, EveryRowShowsItselfOnItsBackAndItsMirrorOnItsBelly) {
  const RolledRows back{
      .height = SHIP_FRAME_ROWS, .count = SHIP_FRAME_ROWS, .belly = false};
  const RolledRows belly{
      .height = SHIP_FRAME_ROWS, .count = SHIP_FRAME_ROWS, .belly = true};

  EXPECT_EQ(sourceRowOf(back, 0), 0);
  EXPECT_EQ(sourceRowOf(back, LAST_ROW_OF_THE_FRAME), LAST_ROW_OF_THE_FRAME);
  EXPECT_EQ(sourceRowOf(belly, 0), LAST_ROW_OF_THE_FRAME);
  EXPECT_EQ(sourceRowOf(belly, LAST_ROW_OF_THE_FRAME), 0);
}

/**
 * Given a ship that does not roll
 * When the share of its roll is taken
 * Then there is none
 */
TEST(BarrelRoll, AShipThatDoesNotRollHasNoShare) {
  EXPECT_FALSE(rollShareOf(ShipMotion{}).has_value());
}

/**
 * Given a ship 200 ms into a roll of 400 ms
 * When the share of its roll is taken
 * Then it is halfway
 */
TEST(BarrelRoll, AShipHalfwayThroughItsRollIsAtHalf) {
  const std::optional<float> share = rollShareOf(ShipMotion{
      .maneuver = ManeuverKind::Roll, .maneuverTime = HALFWAY_THROUGH_A_ROLL});

  ASSERT_TRUE(share.has_value());
  EXPECT_FLOAT_EQ(share.value_or(0.0F), HALF_OF_THE_ROLL);
}

/**
 * Given a ship at the end of its roll, then past it
 * When the share of its roll is taken
 * Then it is whole, never more
 */
TEST(BarrelRoll, AShareNeverPassesTheWholeRoll) {
  const std::optional<float> atTheEnd = rollShareOf(ShipMotion{
      .maneuver = ManeuverKind::Roll, .maneuverTime = END_OF_A_ROLL});
  const std::optional<float> pastTheEnd = rollShareOf(ShipMotion{
      .maneuver = ManeuverKind::Roll, .maneuverTime = PAST_THE_END_OF_A_ROLL});

  EXPECT_FLOAT_EQ(atTheEnd.value_or(0.0F), WHOLE_ROLL);
  EXPECT_FLOAT_EQ(pastTheEnd.value_or(0.0F), WHOLE_ROLL);
}
