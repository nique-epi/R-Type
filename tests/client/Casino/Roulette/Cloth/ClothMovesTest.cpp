#include <gtest/gtest.h>
#include "ClothDirection.hpp"
#include "ClothMoves.hpp"
#include "ClothMovesTestConstants.hpp"

using rtype::client::ClothDirection;
using rtype::client::placeAfter;
using rtype::client::testing::AFTER_SEVENTEEN;
using rtype::client::testing::BOTTOM_COLUMN;
using rtype::client::testing::EVEN_CHANCE;
using rtype::client::testing::FIRST_DOZEN;
using rtype::client::testing::HIGH_CHANCE;
using rtype::client::testing::LOW_CHANCE;
using rtype::client::testing::MIDDLE_COLUMN;
using rtype::client::testing::ONE_PLACE;
using rtype::client::testing::SEVENTEEN_PLACE;
using rtype::client::testing::THIRD_DOZEN;
using rtype::client::testing::THIRTY_FIVE_PLACE;
using rtype::client::testing::THREE_PLACE;
using rtype::client::testing::TWO_PLACE;
using rtype::client::testing::UNDER_ONE;
using rtype::client::testing::UNDER_SEVENTEEN;
using rtype::client::testing::UNDER_THIRTY_FIVE;
using rtype::client::testing::ZERO_FROM_THE_MIDDLE;

/**
 * Given the cursor on 17
 * When it moves right
 * Then it stands on the line after 17, before the next number
 */
TEST(ClothMoves, RightStopsOnTheLineAfterANumber) {
  EXPECT_EQ(placeAfter(SEVENTEEN_PLACE, ClothDirection::Right),
            AFTER_SEVENTEEN);
}

/**
 * Given the cursor on 17
 * When it moves down
 * Then it stands on the line under 17
 */
TEST(ClothMoves, DownStopsOnTheLineUnderANumber) {
  EXPECT_EQ(placeAfter(SEVENTEEN_PLACE, ClothDirection::Down), UNDER_SEVENTEEN);
}

/**
 * Given the cursor on 1, on the bottom row
 * When it moves down, then down again, then down again
 * Then it stands on the street edge, then on the first dozen, then on 1-18
 */
TEST(ClothMoves, DownGoesFromTheStreetToTheDozensToTheChances) {
  EXPECT_EQ(placeAfter(ONE_PLACE, ClothDirection::Down), UNDER_ONE);
  EXPECT_EQ(placeAfter(UNDER_ONE, ClothDirection::Down), FIRST_DOZEN);
  EXPECT_EQ(placeAfter(FIRST_DOZEN, ClothDirection::Down), LOW_CHANCE);
}

/**
 * Given the cursor on EVEN, under the first dozen
 * When it moves up, then up again
 * Then it stands on the first dozen, then on the street edge under 1
 */
TEST(ClothMoves, UpGoesFromTheChancesToTheDozensToTheFirstStreet) {
  EXPECT_EQ(placeAfter(EVEN_CHANCE, ClothDirection::Up), FIRST_DOZEN);
  EXPECT_EQ(placeAfter(FIRST_DOZEN, ClothDirection::Up), UNDER_ONE);
}

/**
 * Given the cursor on 2, on the middle row
 * When it moves left, then right
 * Then it stands on the zero, then on 2 again
 */
TEST(ClothMoves, TheZeroGivesTheCursorBackOnItsRow) {
  const auto onTheZero = placeAfter(TWO_PLACE, ClothDirection::Left);

  EXPECT_EQ(onTheZero, ZERO_FROM_THE_MIDDLE);
  EXPECT_EQ(placeAfter(onTheZero, ClothDirection::Right), TWO_PLACE);
}

/**
 * Given the cursor on 35, then on the line under it
 * When it moves right
 * Then it reaches the column of its row from 35, and stays on the line,
 * where the columns have no place
 */
TEST(ClothMoves, TheColumnsTakeTheCursorOnTheirRowsOnly) {
  EXPECT_EQ(placeAfter(THIRTY_FIVE_PLACE, ClothDirection::Right),
            MIDDLE_COLUMN);
  EXPECT_EQ(placeAfter(UNDER_THIRTY_FIVE, ClothDirection::Right),
            UNDER_THIRTY_FIVE);
}

/**
 * Given the cursor on the middle column, then on the bottom one
 * When it moves down
 * Then it reaches the bottom column, then the third dozen
 */
TEST(ClothMoves, DownTheColumnsSkipsTheLines) {
  EXPECT_EQ(placeAfter(MIDDLE_COLUMN, ClothDirection::Down), BOTTOM_COLUMN);
  EXPECT_EQ(placeAfter(BOTTOM_COLUMN, ClothDirection::Down), THIRD_DOZEN);
}

/**
 * Given the cursor at an end of the cloth: on 3, on the top row; on 19-36
 * When it moves past that end
 * Then it stays where it stands
 */
TEST(ClothMoves, AStepOffTheClothStays) {
  EXPECT_EQ(placeAfter(THREE_PLACE, ClothDirection::Up), THREE_PLACE);
  EXPECT_EQ(placeAfter(HIGH_CHANCE, ClothDirection::Right), HIGH_CHANCE);
  EXPECT_EQ(placeAfter(HIGH_CHANCE, ClothDirection::Down), HIGH_CHANCE);
}
