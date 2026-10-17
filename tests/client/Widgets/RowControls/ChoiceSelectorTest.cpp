#include <gtest/gtest.h>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <string>
#include "BarState.hpp"
#include "ChoiceSelector.hpp"
#include "KeyPress.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "ValueBinding.hpp"

using rtype::client::BarState;
using rtype::client::ChoiceSelector;
using rtype::client::ValueBinding;
using Key = sf::Keyboard::Key;

namespace {

/** @brief Where the selector of COLOR MODE lies on the picture the team
 * chose, a pixel inside its box, and the order of the last color mode. */
constexpr sf::IntRect COLOR_MODE_SELECTOR({316, 72}, {128, 12});
constexpr sf::Vector2i IN_THE_BOX(316 + 60, 78);
constexpr std::size_t LAST_MODE = 3;

class ChoiceSelectorTest : public ::testing::Test {
 protected:
  [[nodiscard]] std::size_t mode() const { return mode_; }
  void setMode(std::size_t mode) { mode_ = mode; }
  [[nodiscard]] int writes() const { return writes_; }
  ChoiceSelector& colorMode() { return colorMode_; }

  /** @returns Whether the selector, painted on a row in a state, writes a
   * text. */
  [[nodiscard]] bool paintingWrites(BarState row,
                                    const std::string& text) const {
    InterfaceCanvasRecord record;
    RecordingInterfaceCanvas canvas(record);
    colorMode_.paint(canvas, COLOR_MODE_SELECTOR, row);
    return writingOf(record, text) != nullptr;
  }

 private:
  std::size_t mode_ = 0;
  int writes_ = 0;
  ChoiceSelector colorMode_{
      {"OFF", "PROTANOPIA", "DEUTERANOPIA", "TRITANOPIA"},
      ValueBinding<std::size_t>{.read = [this] { return mode_; },
                                .write =
                                    [this](std::size_t mode) {
                                      mode_ = mode;
                                      ++writes_;
                                    }}};
};

}  // namespace

/**
 * Given the first color mode
 * When Right is pressed
 * Then the second is chosen
 */
TEST_F(ChoiceSelectorTest, RightChoosesTheNextMode) {
  EXPECT_TRUE(colorMode().keyPressed(keyPressOf(Key::Right)));
  EXPECT_EQ(mode(), 1U);
}

/**
 * Given the last color mode
 * When Right is pressed
 * Then nothing is written: the arrows never wrap
 */
TEST_F(ChoiceSelectorTest, RightOnTheLastModeWritesNothing) {
  setMode(LAST_MODE);
  colorMode().keyPressed(keyPressOf(Key::Right));
  EXPECT_EQ(writes(), 0);
}

/**
 * Given the last color mode
 * When its row is activated, as Enter does
 * Then the first mode is chosen: Enter wraps around
 */
TEST_F(ChoiceSelectorTest, EnterAfterTheLastModeGoesBackToTheFirst) {
  setMode(LAST_MODE);
  colorMode().activate();
  EXPECT_EQ(mode(), 0U);
}

/**
 * Given a mode in the middle of the list
 * When the selector is painted at rest, then selected
 * Then its arrows show on the selected row only
 */
TEST_F(ChoiceSelectorTest, TheArrowsShowOnTheSelectedRowOnly) {
  setMode(1);
  EXPECT_FALSE(paintingWrites(BarState::Idle, "<"));
  EXPECT_TRUE(paintingWrites(BarState::Selected, "<"));
  EXPECT_TRUE(paintingWrites(BarState::Selected, ">"));
}

/**
 * Given the first mode, the row selected
 * When the selector is painted
 * Then the left arrow is hidden, since there is nothing before
 */
TEST_F(ChoiceSelectorTest, TheFirstModeHidesTheLeftArrow) {
  EXPECT_FALSE(paintingWrites(BarState::Selected, "<"));
}

/**
 * Given the first mode
 * When the pointer presses its box
 * Then the next mode is chosen, as with Enter
 */
TEST_F(ChoiceSelectorTest, AClickOnTheBoxDoesAsEnter) {
  EXPECT_TRUE(colorMode().pointerPressed(IN_THE_BOX, COLOR_MODE_SELECTOR));
  EXPECT_EQ(mode(), 1U);
  EXPECT_EQ(colorMode().width(), COLOR_MODE_SELECTOR.size.x);
}
