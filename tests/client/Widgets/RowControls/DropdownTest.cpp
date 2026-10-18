#include <gtest/gtest.h>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include "Dropdown.hpp"
#include "KeyPress.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "ValueBinding.hpp"

using rtype::client::Dropdown;
using rtype::client::ValueBinding;
using Key = sf::Keyboard::Key;

namespace {

/**
 * @brief The list of window sizes on the picture the team chose: its box at
 * (332, 72), 112 by 12; open, the list at (332, 84), 112 by 74, a line of 14
 * pixels per size from (334, 86). On a 1920 x 1080 desktop, the first three
 * sizes fit.
 */
constexpr sf::IntRect WINDOW_SIZE_BOX({332, 72}, {112, 12});
constexpr sf::IntRect OPEN_LIST({332, 84}, {112, 74});
constexpr int FIRST_LINE_MIDDLE = 86 + 7;
constexpr int LINE_HEIGHT = 14;
constexpr std::size_t SIZES_THAT_FIT = 3;
constexpr std::size_t LARGEST_FIT = 2;
constexpr std::size_t TOO_LARGE = 4;
constexpr sf::Vector2i OUTSIDE(100, 100);

sf::Vector2i lineMiddle(std::size_t line) {
  return {OPEN_LIST.position.x + (OPEN_LIST.size.x / 2),
          FIRST_LINE_MIDDLE + (static_cast<int>(line) * LINE_HEIGHT)};
}

class DropdownTest : public ::testing::Test {
 protected:
  bool press(Key key) { return list_.keyPressed(keyPressOf(key)); }

  [[nodiscard]] std::size_t size() const { return size_; }
  Dropdown& list() { return list_; }

 private:
  std::size_t size_ = LARGEST_FIT;
  Dropdown list_{
      {"960 X 540", "1280 X 720", "1600 X 900", "1920 X 1080", "2560 X 1440"},
      [](std::size_t size) { return size < SIZES_THAT_FIT; },
      ValueBinding<std::size_t>{
          .read = [this] { return size_; },
          .write = [this](std::size_t size) { size_ = size; }}};
};

}  // namespace

/**
 * Given the list closed on 1600 x 900
 * When its row is activated, as Enter does
 * Then it opens, holds the input, and highlights the current size
 */
TEST_F(DropdownTest, EnterOpensTheListOnTheCurrentSize) {
  list().activate();
  EXPECT_TRUE(list().isOpen());
  EXPECT_TRUE(list().holdsInput());
  EXPECT_EQ(list().highlighted(), LARGEST_FIT);
}

/**
 * Given the list open on the largest size that fits
 * When Down is pressed
 * Then the highlight stays: the larger sizes cannot be taken, and the list
 * never wraps
 */
TEST_F(DropdownTest, DownStopsAtTheLargestSizeThatFits) {
  list().activate();
  EXPECT_TRUE(press(Key::Down));
  EXPECT_EQ(list().highlighted(), LARGEST_FIT);
}

/**
 * Given the list open on 1600 x 900
 * When Up is pressed
 * Then 1280 x 720 is highlighted, and the size is not changed yet
 */
TEST_F(DropdownTest, UpMovesTheHighlightWithoutApplyingIt) {
  list().activate();
  press(Key::Up);
  EXPECT_EQ(list().highlighted(), 1U);
  EXPECT_EQ(size(), LARGEST_FIT);
}

/**
 * Given 1280 x 720 highlighted
 * When Enter is pressed
 * Then it is the size, and the list closes
 */
TEST_F(DropdownTest, EnterTakesTheHighlightedSize) {
  list().activate();
  press(Key::Up);
  press(Key::Enter);
  EXPECT_EQ(size(), 1U);
  EXPECT_FALSE(list().isOpen());
}

/**
 * Given 1280 x 720 highlighted
 * When Escape is pressed
 * Then the list closes and the size does not change
 */
TEST_F(DropdownTest, EscapeClosesWithoutChanging) {
  list().activate();
  press(Key::Up);
  press(Key::Escape);
  EXPECT_FALSE(list().isOpen());
  EXPECT_EQ(size(), LARGEST_FIT);
}

/**
 * Given 1280 x 720 highlighted
 * When Tab is pressed
 * Then the list closes and the size does not change
 */
TEST_F(DropdownTest, TabClosesWithoutChanging) {
  list().activate();
  press(Key::Up);
  press(Key::Tab);
  EXPECT_FALSE(list().isOpen());
  EXPECT_EQ(size(), LARGEST_FIT);
}

/**
 * Given the list open
 * When Home is pressed
 * Then the first size is highlighted
 */
TEST_F(DropdownTest, HomeHighlightsTheFirstSize) {
  list().activate();
  press(Key::Home);
  EXPECT_EQ(list().highlighted(), 0U);
}

/**
 * Given the list open
 * When the pointer clicks the line of a size too large
 * Then nothing changes and the list stays open
 */
TEST_F(DropdownTest, AClickOnASizeTooLargeTakesNothing) {
  list().activate();
  list().pointerPressed(lineMiddle(TOO_LARGE), WINDOW_SIZE_BOX);
  EXPECT_EQ(size(), LARGEST_FIT);
  EXPECT_TRUE(list().isOpen());
}

/**
 * Given the list open
 * When the pointer clicks the line of 960 x 540
 * Then it is the size, and the list closes
 */
TEST_F(DropdownTest, AClickOnASizeTakesIt) {
  list().activate();
  list().pointerPressed(lineMiddle(0), WINDOW_SIZE_BOX);
  EXPECT_EQ(size(), 0U);
  EXPECT_FALSE(list().isOpen());
}

/**
 * Given the list open
 * When the pointer clicks outside it
 * Then the list closes and the size does not change
 */
TEST_F(DropdownTest, AClickOutsideTheListClosesIt) {
  list().activate();
  list().pointerPressed(OUTSIDE, WINDOW_SIZE_BOX);
  EXPECT_FALSE(list().isOpen());
  EXPECT_EQ(size(), LARGEST_FIT);
}

/**
 * Given the list open
 * When the pointer moves over a size too large
 * Then that size is the one pointed at, and the highlight stays where it was
 */
TEST_F(DropdownTest, ThePointerOverASizeTooLargeHighlightsNothing) {
  list().activate();
  list().pointerMoved(lineMiddle(TOO_LARGE), WINDOW_SIZE_BOX);
  EXPECT_EQ(list().pointedChoice(), TOO_LARGE);
  EXPECT_EQ(list().highlighted(), LARGEST_FIT);
}

/**
 * Given the list closed
 * When Left is pressed
 * Then the key is not used: a size is picked in the list only
 */
TEST_F(DropdownTest, LeftDoesNothingOnTheClosedList) {
  EXPECT_FALSE(press(Key::Left));
}

/**
 * Given the list open
 * When what it lays over the menu is painted
 * Then every pixel of it lies in the list of the picture, under its box, so
 * nothing on its left is covered
 */
TEST_F(DropdownTest, TheOpenListLiesUnderItsBox) {
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
