#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include "LockerChoice.hpp"
#include "LockerChoiceTestConstants.hpp"

using rtype::client::lockerItemAt;
using rtype::client::lockerItemTowards;
using rtype::client::testing::BETWEEN_TWO_CELLS;
using rtype::client::testing::CROWN_CELL;
using rtype::client::testing::CROWN_CELL_MIDDLE;
using rtype::client::testing::EIGHTH_CELL;
using rtype::client::testing::EMPTY_CELL_MIDDLE;
using rtype::client::testing::FIRST_CELL;
using rtype::client::testing::LAST_ITEM_CELL;
using rtype::client::testing::MIDDLE_CELL;
using rtype::client::testing::MIDDLE_LEFT_CELL;
using rtype::client::testing::MIDDLE_RIGHT_CELL;
using rtype::client::testing::NINTH_CELL;
using rtype::client::testing::SECOND_CELL;
using rtype::client::testing::SEVENTH_CELL;
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
 * Given the first cell, in the top left corner
 * When Left or Up is pressed
 * Then the choice stays on it
 */
TEST(LockerChoice, TheChoiceStaysAgainstASide) {
  EXPECT_EQ(lockerItemTowards(FIRST_CELL, Key::Left), FIRST_CELL);
  EXPECT_EQ(lockerItemTowards(FIRST_CELL, Key::Up), FIRST_CELL);
  EXPECT_EQ(lockerItemTowards(CROWN_CELL, Key::Right), CROWN_CELL);
}

/**
 * Given the ninth cell, above an empty cell
 * When Down is pressed
 * Then the choice stays on it
 */
TEST(LockerChoice, TheChoiceNeverGoesDownToAnEmptyCell) {
  EXPECT_EQ(lockerItemTowards(NINTH_CELL, Key::Down), NINTH_CELL);
}

/**
 * Given the last item, left of an empty cell
 * When Right is pressed
 * Then the choice stays on it
 */
TEST(LockerChoice, TheChoiceNeverGoesRightToAnEmptyCell) {
  EXPECT_EQ(lockerItemTowards(LAST_ITEM_CELL, Key::Right), LAST_ITEM_CELL);
}

/**
 * Given the seventh cell, above the last item
 * When Down is pressed
 * Then the choice goes down to the last item
 */
TEST(LockerChoice, TheChoiceGoesDownToTheLastItem) {
  EXPECT_EQ(lockerItemTowards(SEVENTH_CELL, Key::Down), LAST_ITEM_CELL);
}

/**
 * Given any cell
 * When a key that is not an arrow is pressed
 * Then the choice does not move
 */
TEST(LockerChoice, AnotherKeyMovesNothing) {
  EXPECT_EQ(lockerItemTowards(MIDDLE_CELL, Key::Space), std::nullopt);
}

/**
 * Given the middle of the cell of the crown
 * When the item under it is asked
 * Then it is the crown
 */
TEST(LockerChoice, ThePointerOnACellPicksItsItem) {
  EXPECT_EQ(lockerItemAt(CROWN_CELL_MIDDLE), CROWN_CELL);
}

/**
 * Given a pixel between two cells, and one on a cell holding no item
 * When the item under each is asked
 * Then there is none
 */
TEST(LockerChoice, ThePointerOffTheItemsPicksNothing) {
  EXPECT_EQ(lockerItemAt(BETWEEN_TWO_CELLS), std::nullopt);
  EXPECT_EQ(lockerItemAt(EMPTY_CELL_MIDDLE), std::nullopt);
}
