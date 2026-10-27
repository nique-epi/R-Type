#include <gtest/gtest.h>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include "BarState.hpp"
#include "Dropdown.hpp"
#include "DropdownFixture.hpp"
#include "DropdownFixtures.hpp"
#include "DropdownTestConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"

using rtype::client::BarState;
using rtype::client::testing::CURRENT_SIZE_TEXT;
using rtype::client::testing::DISABLED_ROW_BLUE;
using rtype::client::testing::FIRST_SIZE_INDEX;
using rtype::client::testing::IMPOSED_SIZE_TEXT;
using rtype::client::testing::LARGEST_SIZE_INDEX;
using rtype::client::testing::OPEN_LIST;
using rtype::client::testing::OUTSIDE;
using rtype::client::testing::SECOND_SIZE_INDEX;
using rtype::client::testing::TOO_LARGE;
using rtype::client::testing::WINDOW_SIZE_BOX;
using Key = sf::Keyboard::Key;

/**
 * Given the list closed on 1600 x 900
 * When its row is activated, as Enter does
 * Then it opens, holds the input, and highlights the current size
 */
TEST_F(DropdownFixture, EnterOpensTheListOnTheCurrentSize) {
  list().activate();
  EXPECT_TRUE(list().isOpen());
  EXPECT_TRUE(list().holdsInput());
  EXPECT_EQ(list().highlighted(), LARGEST_SIZE_INDEX);
}

/**
 * Given the list open on the largest size that fits
 * When Down is pressed
 * Then the highlight stays: the larger sizes cannot be taken, and the list
 * never wraps
 */
TEST_F(DropdownFixture, DownStopsAtTheLargestSizeThatFits) {
  list().activate();
  EXPECT_TRUE(press(Key::Down));
  EXPECT_EQ(list().highlighted(), LARGEST_SIZE_INDEX);
}

/**
 * Given the list open on 1600 x 900
 * When Up is pressed
 * Then 1280 x 720 is highlighted, and the size is not changed yet
 */
TEST_F(DropdownFixture, UpMovesTheHighlightWithoutApplyingIt) {
  list().activate();
  press(Key::Up);
  EXPECT_EQ(list().highlighted(), SECOND_SIZE_INDEX);
  EXPECT_EQ(size(), LARGEST_SIZE_INDEX);
}

/**
 * Given 1280 x 720 highlighted
 * When Enter is pressed
 * Then it is the size, and the list closes
 */
TEST_F(DropdownFixture, EnterTakesTheHighlightedSize) {
  list().activate();
  press(Key::Up);
  press(Key::Enter);
  EXPECT_EQ(size(), SECOND_SIZE_INDEX);
  EXPECT_FALSE(list().isOpen());
}

/**
 * Given 1280 x 720 highlighted
 * When Escape is pressed
 * Then the list closes and the size does not change
 */
TEST_F(DropdownFixture, EscapeClosesWithoutChanging) {
  list().activate();
  press(Key::Up);
  press(Key::Escape);
  EXPECT_FALSE(list().isOpen());
  EXPECT_EQ(size(), LARGEST_SIZE_INDEX);
}

/**
 * Given 1280 x 720 highlighted
 * When Tab is pressed
 * Then the list closes and the size does not change
 */
TEST_F(DropdownFixture, TabClosesWithoutChanging) {
  list().activate();
  press(Key::Up);
  press(Key::Tab);
  EXPECT_FALSE(list().isOpen());
  EXPECT_EQ(size(), LARGEST_SIZE_INDEX);
}

/**
 * Given the list open
 * When Home is pressed
 * Then the first size is highlighted
 */
TEST_F(DropdownFixture, HomeHighlightsTheFirstSize) {
  list().activate();
  press(Key::Home);
  EXPECT_EQ(list().highlighted(), FIRST_SIZE_INDEX);
}

/**
 * Given the list open
 * When the pointer clicks the line of a size too large
 * Then nothing changes and the list stays open
 */
TEST_F(DropdownFixture, AClickOnASizeTooLargeTakesNothing) {
  list().activate();
  list().pointerPressed(lineMiddle(TOO_LARGE), WINDOW_SIZE_BOX);
  EXPECT_EQ(size(), LARGEST_SIZE_INDEX);
  EXPECT_TRUE(list().isOpen());
}

/**
 * Given the list open
 * When the pointer clicks the line of 960 x 540
 * Then it is the size, and the list closes
 */
TEST_F(DropdownFixture, AClickOnASizeTakesIt) {
  list().activate();
  list().pointerPressed(lineMiddle(FIRST_SIZE_INDEX), WINDOW_SIZE_BOX);
  EXPECT_EQ(size(), FIRST_SIZE_INDEX);
  EXPECT_FALSE(list().isOpen());
}

/**
 * Given the list open
 * When the pointer clicks outside it
 * Then the list closes and the size does not change
 */
TEST_F(DropdownFixture, AClickOutsideTheListClosesIt) {
  list().activate();
  list().pointerPressed(OUTSIDE, WINDOW_SIZE_BOX);
  EXPECT_FALSE(list().isOpen());
  EXPECT_EQ(size(), LARGEST_SIZE_INDEX);
}

/**
 * Given the list open
 * When the pointer moves over a size too large
 * Then that size is the one pointed at, and the highlight stays where it was
 */
TEST_F(DropdownFixture, ThePointerOverASizeTooLargeHighlightsNothing) {
  list().activate();
  list().pointerMoved(lineMiddle(TOO_LARGE), WINDOW_SIZE_BOX);
  EXPECT_EQ(list().pointedChoice(), TOO_LARGE);
  EXPECT_EQ(list().highlighted(), LARGEST_SIZE_INDEX);
}

/**
 * Given the list closed
 * When Left is pressed
 * Then the key is not used: a size is picked in the list only
 */
TEST_F(DropdownFixture, LeftDoesNothingOnTheClosedList) {
  EXPECT_FALSE(press(Key::Left));
}

/**
 * Given the list open
 * When what it lays over the menu is painted
 * Then every pixel of it lies in the list of the picture, under its box, so
 * nothing on its left is covered
 */
TEST_F(DropdownFixture, TheOpenListLiesUnderItsBox) {
  list().activate();
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  list().paintOverlay(canvas, WINDOW_SIZE_BOX);
  ASSERT_FALSE(record.fills.empty());
  for (const RecordedFill& fill : record.fills) {
    EXPECT_TRUE(OPEN_LIST.contains(fill.area.position));
    EXPECT_TRUE(OPEN_LIST.contains(fill.area.position + fill.area.size -
                                   sf::Vector2i(1, 1)));
  }
}

/**
 * Given the list on 1600 x 900, the size of a 1920 x 1080 screen imposed
 * When its box is painted
 * Then the box shows 1920 x 1080, and not 1600 x 900
 */
TEST_F(DropdownFixture, TheBoxShowsTheImposedTextInPlaceOfTheChoice) {
  impose(IMPOSED_SIZE_TEXT);
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);

  list().paint(canvas, WINDOW_SIZE_BOX, BarState::Idle);

  EXPECT_NE(writingOf(record, IMPOSED_SIZE_TEXT), nullptr);
  EXPECT_EQ(writingOf(record, CURRENT_SIZE_TEXT), nullptr);
}

/**
 * Given the list on 1600 x 900
 * When its box is painted on a disabled row
 * Then its text is the blue of a disabled row
 */
TEST_F(DropdownFixture, ADisabledRowDimsTheTextOfTheBox) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);

  list().paint(canvas, WINDOW_SIZE_BOX, BarState::Disabled);

  const RecordedWriting* text = writingOf(record, CURRENT_SIZE_TEXT);
  ASSERT_NE(text, nullptr);
  EXPECT_EQ(text->style.color, DISABLED_ROW_BLUE);
}
