#include <gtest/gtest.h>
#include <cstddef>
#include <optional>
#include "BodyPlacement.hpp"
#include "VolleyPlacement.hpp"
#include "VolleyPlacementFixtures.hpp"
#include "VolleyPlacementTestConstants.hpp"

using rtype::game::BodyPlacement;
using rtype::game::volleyPlacement;
using rtype::game::testing::BURST_COUNT;
using rtype::game::testing::BURST_ORIGIN;
using rtype::game::testing::burstOf;
using rtype::game::testing::column;
using rtype::game::testing::COLUMN_CENTER;
using rtype::game::testing::FIRST_WALL_BODY;
using rtype::game::testing::FOURTH_WALL_BODY;
using rtype::game::testing::LAST_BURST_BODY;
using rtype::game::testing::LAST_BURST_BODY_POSITION;
using rtype::game::testing::LONE_BODY_COUNT;
using rtype::game::testing::LONE_BODY_POSITION;
using rtype::game::testing::MIDDLE_BURST_BODY;
using rtype::game::testing::MIDDLE_BURST_BODY_POSITION;
using rtype::game::testing::PLACEMENT_TOLERANCE_UNITS;
using rtype::game::testing::SECOND_BURST_BODY;
using rtype::game::testing::SECOND_BURST_BODY_POSITION;
using rtype::game::testing::SECOND_WALL_BODY;
using rtype::game::testing::THIRD_COLUMN_BODY;
using rtype::game::testing::THIRD_COLUMN_BODY_POSITION;
using rtype::game::testing::THIRD_WALL_BODY;
using rtype::game::testing::TYRANT_AT_REST_CENTER;
using rtype::game::testing::wallWithAGap;

/**
 * Given the burst of five bodies of the Tyrant at rest
 * When the second, the middle and the last body are placed
 * Then they head -12, 0 and 24 degrees, 8 units from the mouth
 */
TEST(VolleyPlacement, AFanSpreadsItsBodiesEvenly) {
  const std::optional<BodyPlacement> second = volleyPlacement(
      burstOf(BURST_COUNT), TYRANT_AT_REST_CENTER, SECOND_BURST_BODY);
  const std::optional<BodyPlacement> middle = volleyPlacement(
      burstOf(BURST_COUNT), TYRANT_AT_REST_CENTER, MIDDLE_BURST_BODY);
  const std::optional<BodyPlacement> last = volleyPlacement(
      burstOf(BURST_COUNT), TYRANT_AT_REST_CENTER, LAST_BURST_BODY);

  ASSERT_TRUE(second.has_value() && middle.has_value() && last.has_value());
  const BodyPlacement secondFound = second.value_or(BodyPlacement{});
  const BodyPlacement middleFound = middle.value_or(BodyPlacement{});
  const BodyPlacement lastFound = last.value_or(BodyPlacement{});
  EXPECT_NEAR(secondFound.position.x, SECOND_BURST_BODY_POSITION.x,
              PLACEMENT_TOLERANCE_UNITS);
  EXPECT_NEAR(secondFound.position.y, SECOND_BURST_BODY_POSITION.y,
              PLACEMENT_TOLERANCE_UNITS);
  EXPECT_NEAR(middleFound.position.x, MIDDLE_BURST_BODY_POSITION.x,
              PLACEMENT_TOLERANCE_UNITS);
  EXPECT_NEAR(middleFound.position.y, MIDDLE_BURST_BODY_POSITION.y,
              PLACEMENT_TOLERANCE_UNITS);
  EXPECT_NEAR(lastFound.position.x, LAST_BURST_BODY_POSITION.x,
              PLACEMENT_TOLERANCE_UNITS);
  EXPECT_NEAR(lastFound.position.y, LAST_BURST_BODY_POSITION.y,
              PLACEMENT_TOLERANCE_UNITS);
}

/**
 * Given the burst of five bodies of the Tyrant at rest
 * When each body is placed
 * Then every body steers from the mouth, (1492, 536)
 */
TEST(VolleyPlacement, AFanSteersFromItsStart) {
  for (std::size_t index = 0; index < BURST_COUNT; ++index) {
    const std::optional<BodyPlacement> placed =
        volleyPlacement(burstOf(BURST_COUNT), TYRANT_AT_REST_CENTER, index);
    ASSERT_TRUE(placed.has_value());
    const BodyPlacement found = placed.value_or(BodyPlacement{});
    EXPECT_FLOAT_EQ(found.origin.x, BURST_ORIGIN.x);
    EXPECT_FLOAT_EQ(found.origin.y, BURST_ORIGIN.y);
  }
}

/**
 * Given a column of three bodies 50 units apart
 * When its third body is placed
 * Then it appears two steps down from the first: (900, 600)
 */
TEST(VolleyPlacement, AColumnStepsDown) {
  const std::optional<BodyPlacement> third =
      volleyPlacement(column(), COLUMN_CENTER, THIRD_COLUMN_BODY);

  ASSERT_TRUE(third.has_value());
  const BodyPlacement found = third.value_or(BodyPlacement{});
  EXPECT_FLOAT_EQ(found.position.x, THIRD_COLUMN_BODY_POSITION.x);
  EXPECT_FLOAT_EQ(found.position.y, THIRD_COLUMN_BODY_POSITION.y);
}

/**
 * Given a wall of nine bodies whose second and third are its gap
 * When its first four bodies are placed
 * Then the first and the fourth appear, the second and the third do not
 */
TEST(VolleyPlacement, ABodyOfTheGapIsLeftOut) {
  EXPECT_TRUE(volleyPlacement(wallWithAGap(), COLUMN_CENTER, FIRST_WALL_BODY)
                  .has_value());
  EXPECT_FALSE(volleyPlacement(wallWithAGap(), COLUMN_CENTER, SECOND_WALL_BODY)
                   .has_value());
  EXPECT_FALSE(volleyPlacement(wallWithAGap(), COLUMN_CENTER, THIRD_WALL_BODY)
                   .has_value());
  EXPECT_TRUE(volleyPlacement(wallWithAGap(), COLUMN_CENTER, FOURTH_WALL_BODY)
                  .has_value());
}

/**
 * Given a burst of one body, spread over 48 degrees
 * When it is placed
 * Then it heads left, 8 units from the mouth: a lone body has no spread
 */
TEST(VolleyPlacement, AVolleyOfOneHeadsLeft) {
  const std::optional<BodyPlacement> placed =
      volleyPlacement(burstOf(LONE_BODY_COUNT), TYRANT_AT_REST_CENTER, 0);

  ASSERT_TRUE(placed.has_value());
  const BodyPlacement found = placed.value_or(BodyPlacement{});
  EXPECT_FLOAT_EQ(found.position.x, LONE_BODY_POSITION.x);
  EXPECT_FLOAT_EQ(found.position.y, LONE_BODY_POSITION.y);
}
