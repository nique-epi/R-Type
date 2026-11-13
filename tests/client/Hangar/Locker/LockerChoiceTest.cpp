#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include "LockerChoice.hpp"
#include "LockerChoiceTestConstants.hpp"

using rtype::client::lockerItemAt;
using rtype::client::lockerItemShownIn;
using rtype::client::lockerItemTowards;
using rtype::client::lockerRowOf;
using rtype::client::testing::ABOVE_THE_LAST_ITEM;
using rtype::client::testing::BETWEEN_TWO_CELLS;
using rtype::client::testing::CROWN_CELL;
using rtype::client::testing::CROWN_CELL_MIDDLE;
using rtype::client::testing::EIGHTH_CELL;
using rtype::client::testing::FIFTH_ROW_CELL;
using rtype::client::testing::FIRST_CELL;
using rtype::client::testing::FIRST_COLUMN_OF_THE_SEVENTH_ROW;
using rtype::client::testing::FIRST_ROWS;
using rtype::client::testing::FOURTH_ROW_CELL;
using rtype::client::testing::FROM_THE_FIRST_ROW;
using rtype::client::testing::FROM_THE_THIRD_ROW;
using rtype::client::testing::LAST_COLUMN_OF_THE_FOURTH_ROW;
using rtype::client::testing::LAST_ITEM;
using rtype::client::testing::LAST_ROWS;
using rtype::client::testing::MIDDLE_CELL;
using rtype::client::testing::MIDDLE_LEFT_CELL;
using rtype::client::testing::MIDDLE_RIGHT_CELL;
using rtype::client::testing::ROW_OF_THE_LAST_ITEM;
using rtype::client::testing::SECOND_CELL;
using rtype::client::testing::UNDER_THE_CELLS;
using rtype::client::testing::UNDER_THE_CROWN_CELL_FROM_THE_THIRD_ROW;
using Key = sf::Keyboard::Key;

/**
 * Given the middle cell of the locker
 * When each arrow moves the choice
 * Then it goes to the cell on that side
 */
TEST(LockerChoice, AnArrowMovesToTheNeighbourCell) {
  EXPECT_EQ(lockerItemTowards(MIDDLE_CELL, Key::Left), MIDDLE_LEFT_CELL);
  EXPECT_EQ(lockerItemTowards(MIDDLE_CELL, Key::Right), MIDDLE_RIGHT_CELL);
  EXPECT_EQ(lockerItemTowards(MIDDLE_CELL, Key::Up), SECOND_CELL);
  EXPECT_EQ(lockerItemTowards(MIDDLE_CELL, Key::Down), EIGHTH_CELL);
}

/**
 * Given a cell against a side of the locker
 * When the arrow of that side is pressed
 * Then the choice stays on it
 */
TEST(LockerChoice, TheChoiceStaysAgainstASide) {
  EXPECT_EQ(lockerItemTowards(FIRST_CELL, Key::Left), FIRST_CELL);
  EXPECT_EQ(lockerItemTowards(FIRST_CELL, Key::Up), FIRST_CELL);
  EXPECT_EQ(lockerItemTowards(CROWN_CELL, Key::Right), CROWN_CELL);
}

/**
 * Given the first cell of the fourth row, the last row shown at first
 * When Down is pressed
 * Then the choice goes to the fifth row, not shown yet
 */
TEST(LockerChoice, DownGoesPastTheRowsShown) {
  EXPECT_EQ(lockerItemTowards(FOURTH_ROW_CELL, Key::Down), FIFTH_ROW_CELL);
}

/**
 * Given the last item, in the bottom right corner of the last row
 * When Down or Right is pressed
 * Then the choice stays on it; Down from the item above reaches it
 */
TEST(LockerChoice, TheChoiceStaysOnTheLastItem) {
  EXPECT_EQ(lockerItemTowards(LAST_ITEM, Key::Down), LAST_ITEM);
  EXPECT_EQ(lockerItemTowards(LAST_ITEM, Key::Right), LAST_ITEM);
  EXPECT_EQ(lockerItemTowards(ABOVE_THE_LAST_ITEM, Key::Down), LAST_ITEM);
}

/**
 * Given a cell of the locker
 * When a key that is no arrow is pressed
 * Then nothing is chosen
 */
TEST(LockerChoice, AnotherKeyMovesNothing) {
  EXPECT_EQ(lockerItemTowards(MIDDLE_CELL, Key::Space), std::nullopt);
}

/**
 * Given the middle of the cell of the crown, the locker at its first rows
 * When the item under it is asked
 * Then it is the crown
 */
TEST(LockerChoice, ThePointerOnACellPicksItsItem) {
  EXPECT_EQ(lockerItemAt(CROWN_CELL_MIDDLE, FROM_THE_FIRST_ROW), CROWN_CELL);
}

/**
 * Given the same pixel, the locker scrolled two rows down
 * When the item under it is asked
 * Then it is the item two rows under the crown
 */
TEST(LockerChoice, ThePointerPicksTheItemOfTheRowsShown) {
  EXPECT_EQ(lockerItemAt(CROWN_CELL_MIDDLE, FROM_THE_THIRD_ROW),
            UNDER_THE_CROWN_CELL_FROM_THE_THIRD_ROW);
}

/**
 * Given a pixel between two cells, and one under the last row shown
 * When the item under each is asked
 * Then there is none
 */
TEST(LockerChoice, ThePointerOffTheCellsPicksNothing) {
  EXPECT_EQ(lockerItemAt(BETWEEN_TWO_CELLS, FROM_THE_FIRST_ROW), std::nullopt);
  EXPECT_EQ(lockerItemAt(UNDER_THE_CELLS, FROM_THE_FIRST_ROW), std::nullopt);
}

/**
 * Given the last item and the first one
 * When the row of the last and the choices kept among other rows are asked
 * Then the last item is in the tenth row, and a hidden item is kept in its
 * column in the nearest row shown
 */
TEST(LockerChoice, AHiddenChoiceIsKeptInTheNearestRowShown) {
  EXPECT_EQ(lockerRowOf(LAST_ITEM), ROW_OF_THE_LAST_ITEM);
  EXPECT_EQ(lockerItemShownIn(FIRST_CELL, LAST_ROWS),
            FIRST_COLUMN_OF_THE_SEVENTH_ROW);
  EXPECT_EQ(lockerItemShownIn(LAST_ITEM, FIRST_ROWS),
            LAST_COLUMN_OF_THE_FOURTH_ROW);
  EXPECT_EQ(lockerItemShownIn(CROWN_CELL, FIRST_ROWS), CROWN_CELL);
}
