#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <optional>
#include "KeyPress.hpp"
#include "Menu.hpp"
#include "MenuBuilderDialogRowsFixtures.hpp"
#include "MenuBuilderDialogRowsTestConstants.hpp"
#include "MenuPainting.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingRowControl.hpp"

using rtype::client::Menu;
using rtype::client::testing::FIRST_ROW_LABEL;
using rtype::client::testing::SECOND_ROW_LABEL;

/**
 * Given a dialog holding two rows
 * When it is painted
 * Then its rows stand one under the other, under its heading
 */
TEST(MenuBuilderDialogRows, RowsStandUnderTheHeading) {
  RowControlRecord first;
  RowControlRecord second;
  const InterfaceCanvasRecord painting =
      paintingOf(dialogWithRows(first, second));

  EXPECT_EQ(anchorOf(painting, "FIRST"), FIRST_ROW_LABEL);
  EXPECT_EQ(anchorOf(painting, "SECOND"), SECOND_ROW_LABEL);
}

/**
 * Given a dialog holding two rows, the first one selected
 * When the right arrow is pressed
 * Then the row gets it, and the selection stays on that row
 */
TEST(MenuBuilderDialogRows, RightStepsTheValueOfTheSelectedRow) {
  RowControlRecord first;
  RowControlRecord second;
  Menu menu = dialogWithRows(first, second);

  menu.handleEvent(keyPress(sf::Keyboard::Key::Right), samePlayfieldPoint);

  EXPECT_EQ(first.keys.size(), 1);
  EXPECT_EQ(menu.selectedButton(), std::optional<std::size_t>(0));
}

/**
 * Given a dialog holding two rows, the first one selected
 * When the left arrow is pressed, which its control does not use
 * Then the selection stays: only up, down and Tab move between the bars
 */
TEST(MenuBuilderDialogRows, AnArrowTheRowDoesNotUseMovesNothing) {
  RowControlRecord first;
  RowControlRecord second;
  Menu menu = dialogWithRows(first, second);

  menu.handleEvent(keyPress(sf::Keyboard::Key::Left), samePlayfieldPoint);

  EXPECT_EQ(menu.selectedButton(), std::optional<std::size_t>(0));
}

/**
 * Given a dialog without a row, its first button selected
 * When the right arrow is pressed
 * Then the next button is selected: only rows take the left and right arrows
 * away from the bars
 */
TEST(MenuBuilderDialogRows, ADialogWithoutRowsKeepsItsArrows) {
  Menu menu = dialogWithoutRows();

  menu.handleEvent(keyPress(sf::Keyboard::Key::Right), samePlayfieldPoint);

  EXPECT_EQ(menu.selectedButton(), std::optional<std::size_t>(1));
}

/**
 * Given a dialog holding two rows, the first one selected
 * When the down arrow is pressed
 * Then the second row is selected
 */
TEST(MenuBuilderDialogRows, DownMovesToTheNextRow) {
  RowControlRecord first;
  RowControlRecord second;
  Menu menu = dialogWithRows(first, second);

  menu.handleEvent(keyPress(sf::Keyboard::Key::Down), samePlayfieldPoint);

  EXPECT_EQ(menu.selectedButton(), std::optional<std::size_t>(1));
}
