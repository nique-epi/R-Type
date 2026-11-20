#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <span>
#include <vector>
#include "BulbChase.hpp"
#include "BulbChaseTestConstants.hpp"
#include "ColoredPixel.hpp"

using rtype::client::BulbChase;
using rtype::client::ColoredPixel;
using rtype::client::testing::A_WHOLE_TURN;
using rtype::client::testing::BULB_BELOW_ZERO;
using rtype::client::testing::FIRST_BULB;
using rtype::client::testing::FIRST_RANK;
using rtype::client::testing::FIRST_STEP;
using rtype::client::testing::NEXT_STEP;
using rtype::client::testing::RANK_BELOW_ZERO;
using rtype::client::testing::SECOND_BULB;
using rtype::client::testing::SECOND_RANK;
using rtype::client::testing::THIRD_BULB;
using rtype::client::testing::THIRD_RANK;
using rtype::client::testing::UNLIT_BULB;
using testing::ElementsAre;
using testing::Field;
using testing::UnorderedElementsAre;

/**
 * Given a chase of three bulbs one rank apart
 * When its first step is read
 * Then only the bulb of rank 0 is out, in the color of a bulb out
 */
TEST(BulbChase, TheFirstStepPutsOutTheBulbOfRankZero) {
  BulbChase chase(UNLIT_BULB);
  chase.add(FIRST_BULB, FIRST_RANK);
  chase.add(SECOND_BULB, SECOND_RANK);
  chase.add(THIRD_BULB, THIRD_RANK);

  const std::span<const ColoredPixel> unlit = chase.unlitAt(FIRST_STEP);

  ASSERT_EQ(unlit.size(), 1U);
  EXPECT_EQ(unlit.front().position, FIRST_BULB);
  EXPECT_EQ(unlit.front().color, UNLIT_BULB);
}

/**
 * Given a chase of three bulbs one rank apart
 * When it moves one step on
 * Then the bulb one rank lower is out, the lowest wrapping to the highest
 */
TEST(BulbChase, TheNextStepPutsOutTheBulbOneRankLower) {
  BulbChase chase(UNLIT_BULB);
  chase.add(FIRST_BULB, FIRST_RANK);
  chase.add(SECOND_BULB, SECOND_RANK);
  chase.add(THIRD_BULB, THIRD_RANK);

  const std::span<const ColoredPixel> unlit = chase.unlitAt(NEXT_STEP);

  EXPECT_THAT(std::vector<ColoredPixel>(unlit.begin(), unlit.end()),
              ElementsAre(Field(&ColoredPixel::position, THIRD_BULB)));
}

/**
 * Given a chase holding a bulb of rank 2 and one of rank -1
 * When the step that puts out rank 2 is read
 * Then the bulb below zero is out with it, sharing its phase
 */
TEST(BulbChase, ARankBelowZeroSharesThePhaseOfItsRemainder) {
  BulbChase chase(UNLIT_BULB);
  chase.add(THIRD_BULB, THIRD_RANK);
  chase.add(BULB_BELOW_ZERO, RANK_BELOW_ZERO);

  const std::span<const ColoredPixel> unlit = chase.unlitAt(NEXT_STEP);

  EXPECT_THAT(
      std::vector<ColoredPixel>(unlit.begin(), unlit.end()),
      UnorderedElementsAre(Field(&ColoredPixel::position, THIRD_BULB),
                           Field(&ColoredPixel::position, BULB_BELOW_ZERO)));
}

/**
 * Given a chase of three bulbs one rank apart
 * When a whole turn of its phases has passed
 * Then the bulb of the first step is out again
 */
TEST(BulbChase, AWholeTurnBringsTheFirstStepBack) {
  BulbChase chase(UNLIT_BULB);
  chase.add(FIRST_BULB, FIRST_RANK);
  chase.add(SECOND_BULB, SECOND_RANK);
  chase.add(THIRD_BULB, THIRD_RANK);

  const std::span<const ColoredPixel> unlit = chase.unlitAt(A_WHOLE_TURN);

  EXPECT_THAT(std::vector<ColoredPixel>(unlit.begin(), unlit.end()),
              ElementsAre(Field(&ColoredPixel::position, FIRST_BULB)));
}
