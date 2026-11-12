#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include "ShopSelection.hpp"
#include "ShopSelectionFixtures.hpp"
#include "ShopSelectionTestConstants.hpp"

using rtype::client::ShopSelection;
using rtype::client::testing::BETWEEN_TWO_CELLS;
using rtype::client::testing::FIRST_CELL_MIDDLE;
using rtype::client::testing::FIRST_HULL;
using rtype::client::testing::FIRST_LOOK;
using rtype::client::testing::FIRST_STALL_MIDDLE;
using rtype::client::testing::FOUR_ROWS;
using rtype::client::testing::FOURTH_LOOK;
using rtype::client::testing::FULL_SHELVES;
using rtype::client::testing::LAST_HULL;
using rtype::client::testing::LAST_LOOK;
using rtype::client::testing::LAST_ROW_SECOND_CELL_MIDDLE;
using rtype::client::testing::MORE_THAN_THE_HULLS;
using rtype::client::testing::moveBy;
using rtype::client::testing::SECOND_LOOK;
using rtype::client::testing::SECOND_ROW;
using rtype::client::testing::SEVEN_ROWS;
using rtype::client::testing::SHELVES_WITHOUT_HULLS;
using rtype::client::testing::SIX_ROWS;
using rtype::client::testing::THIRD_LOOK;
using Key = sf::Keyboard::Key;

/**
 * Given a shop just opened
 * When nothing is pressed
 * Then the first look of the locker is chosen
 */
TEST(ShopSelection, StartsOnTheFirstLook) {
  const ShopSelection selection(FULL_SHELVES);

  EXPECT_EQ(selection.spot(), FIRST_LOOK);
}

/**
 * Given the first look chosen
 * When Right is pressed
 * Then the second look is chosen
 */
TEST(ShopSelection, RightGoesToTheNextLook) {
  ShopSelection selection(FULL_SHELVES);

  moveBy(selection, Key::Right, 1);

  EXPECT_EQ(selection.spot(), SECOND_LOOK);
}

/**
 * Given the first look chosen
 * When Down is pressed
 * Then the look under it is chosen
 */
TEST(ShopSelection, DownGoesToTheLookUnder) {
  ShopSelection selection(FULL_SHELVES);

  moveBy(selection, Key::Down, 1);

  EXPECT_EQ(selection.spot(), FOURTH_LOOK);
}

/**
 * Given the first look chosen, against the left and top sides
 * When Left then Up are pressed
 * Then the choice stays on it
 */
TEST(ShopSelection, TheChoiceStaysAgainstTheSidesOfTheLocker) {
  ShopSelection selection(FULL_SHELVES);

  moveBy(selection, Key::Left, 1);
  moveBy(selection, Key::Up, 1);

  EXPECT_EQ(selection.spot(), FIRST_LOOK);
}

/**
 * Given the third look chosen, in the right column
 * When Right is pressed
 * Then the cheapest hull of the case is chosen
 */
TEST(ShopSelection, RightFromTheRightColumnEntersTheCase) {
  ShopSelection selection(FULL_SHELVES);
  moveBy(selection, Key::Right, 2);

  moveBy(selection, Key::Right, 1);

  EXPECT_EQ(selection.spot(), FIRST_HULL);
}

/**
 * Given a shop without hulls, the third look chosen
 * When Right is pressed
 * Then the choice stays on it: there is no case to enter
 */
TEST(ShopSelection, RightStaysInALockerWithoutACase) {
  ShopSelection selection(SHELVES_WITHOUT_HULLS);
  moveBy(selection, Key::Right, 2);

  moveBy(selection, Key::Right, 1);

  EXPECT_EQ(selection.spot(), THIRD_LOOK);
}

/**
 * Given the first four rows of the locker in sight
 * When Down is pressed down to the fifth row
 * Then the rows scroll by one, so the chosen look shows
 */
TEST(ShopSelection, DownPastTheLastShownRowScrollsTheLocker) {
  ShopSelection selection(FULL_SHELVES);

  moveBy(selection, Key::Down, FOUR_ROWS);

  EXPECT_EQ(selection.window().first, SECOND_ROW);
}

/**
 * Given the middle look of the seventh row, over a hole of the last row
 * When Down is pressed
 * Then the last look, alone on its row, is chosen
 */
TEST(ShopSelection, DownOverAHoleGoesToTheLastLook) {
  ShopSelection selection(FULL_SHELVES);
  moveBy(selection, Key::Down, SIX_ROWS);
  moveBy(selection, Key::Right, 1);

  moveBy(selection, Key::Down, 1);

  EXPECT_EQ(selection.spot(), LAST_LOOK);
}

/**
 * Given the last look chosen, on the last row
 * When Down is pressed
 * Then the cheapest hull of the case is chosen
 */
TEST(ShopSelection, DownFromTheLastRowEntersTheCase) {
  ShopSelection selection(FULL_SHELVES);
  moveBy(selection, Key::Down, SEVEN_ROWS);

  moveBy(selection, Key::Down, 1);

  EXPECT_EQ(selection.spot(), FIRST_HULL);
}

/**
 * Given the case entered from the third look, the second hull chosen
 * When Up is pressed
 * Then the third look is chosen again
 */
TEST(ShopSelection, UpFromTheCaseComesBackToTheLookItLeft) {
  ShopSelection selection(FULL_SHELVES);
  moveBy(selection, Key::Right, 4);

  moveBy(selection, Key::Up, 1);

  EXPECT_EQ(selection.spot(), THIRD_LOOK);
}

/**
 * Given the case entered from the third look, its first hull chosen
 * When Left is pressed
 * Then the third look is chosen again
 */
TEST(ShopSelection, LeftFromTheFirstHullComesBackToTheLookItLeft) {
  ShopSelection selection(FULL_SHELVES);
  moveBy(selection, Key::Right, 3);

  moveBy(selection, Key::Left, 1);

  EXPECT_EQ(selection.spot(), THIRD_LOOK);
}

/**
 * Given the case entered
 * When Right is pressed more times than it has hulls
 * Then the choice stays on the dearest hull
 */
TEST(ShopSelection, TheCaseStopsAtItsLastHull) {
  ShopSelection selection(FULL_SHELVES);

  moveBy(selection, Key::Right, MORE_THAN_THE_HULLS);

  EXPECT_EQ(selection.spot(), LAST_HULL);
}

/**
 * Given the locker scrolled by one row
 * When the pointer lies over its first shown cell
 * Then the first look of the second row is chosen
 */
TEST(ShopSelection, ThePointerChoosesTheLookOfTheShownCell) {
  ShopSelection selection(FULL_SHELVES);
  selection.scroll(1);

  EXPECT_TRUE(selection.chooseAt(FIRST_CELL_MIDDLE));

  EXPECT_EQ(selection.spot(), FOURTH_LOOK);
}

/**
 * Given the locker scrolled to its last rows, the last one holding one look
 * When the pointer lies over the second cell of that row, which is empty
 * Then nothing is chosen there
 */
TEST(ShopSelection, ThePointerOverAnEmptyCellChoosesNothing) {
  ShopSelection selection(FULL_SHELVES);
  selection.scroll(FOUR_ROWS);

  EXPECT_FALSE(selection.chooseAt(LAST_ROW_SECOND_CELL_MIDDLE));
}

/**
 * Given the locker
 * When the pointer lies between two cells
 * Then nothing is chosen there
 */
TEST(ShopSelection, ThePointerBetweenTwoCellsChoosesNothing) {
  ShopSelection selection(FULL_SHELVES);

  EXPECT_FALSE(selection.chooseAt(BETWEEN_TWO_CELLS));
}

/**
 * Given the locker
 * When the pointer lies over the first stall of the case
 * Then the cheapest hull is chosen
 */
TEST(ShopSelection, ThePointerChoosesTheHullOfAStall) {
  ShopSelection selection(FULL_SHELVES);

  EXPECT_TRUE(selection.chooseAt(FIRST_STALL_MIDDLE));

  EXPECT_EQ(selection.spot(), FIRST_HULL);
}

/**
 * Given the first look chosen, on the first row
 * When the wheel scrolls the locker down by one row
 * Then the look under it is chosen, so the choice stays in sight
 */
TEST(ShopSelection, ScrollingKeepsTheChosenLookInSight) {
  ShopSelection selection(FULL_SHELVES);

  selection.scroll(1);

  EXPECT_EQ(selection.spot(), FOURTH_LOOK);
}
