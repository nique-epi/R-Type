#include <gtest/gtest.h>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include "BarState.hpp"
#include "KeyPress.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "Slider.hpp"
#include "ValueBinding.hpp"

using rtype::client::BarState;
using rtype::client::Slider;
using rtype::client::ValueBinding;
using Key = sf::Keyboard::Key;

namespace {

/** @brief The volumes of the options: 0 to 100 by 5, a page of 10, and the
 * default of the music. */
constexpr Slider::Range PERCENT{
    .minimum = 0, .maximum = 100, .step = 5, .pageSteps = 2};
constexpr int MUSIC_DEFAULT = 70;
constexpr int MAXIMUM_OF_THE_OPTIONS = 100;
constexpr int VOLUME_STEP_OF_THE_OPTIONS = 5;

/** @brief The slider of the first row of the options, on the picture the
 * team chose, and its thumb, 6 by 10. */
constexpr sf::IntRect FIRST_ROW_SLIDER({276, 72}, {168, 12});
constexpr sf::Vector2i THUMB(6, 10);
constexpr sf::Vector2i LEFT_OF_THE_TRACK(276 + 1, 78);
constexpr sf::Vector2i RIGHT_OF_THE_TRACK(276 + 131, 78);
constexpr sf::Vector2i ON_THE_VALUE(276 + 150, 78);

class SliderTest : public ::testing::Test {
 protected:
  bool press(Key key) { return slider_.keyPressed(keyPressOf(key)); }

  [[nodiscard]] int value() const { return value_; }
  void setValue(int value) { value_ = value; }
  [[nodiscard]] int writes() const { return writes_; }
  void mute() { muted_ = true; }
  Slider& slider() { return slider_; }

  [[nodiscard]] InterfaceCanvasRecord painting(BarState row) const {
    InterfaceCanvasRecord record;
    RecordingInterfaceCanvas canvas(record);
    slider_.paint(canvas, FIRST_ROW_SLIDER, row);
    return record;
  }

  /** @returns The middle of the thumb as it is drawn. */
  [[nodiscard]] sf::Vector2i thumbMiddle() const {
    const InterfaceCanvasRecord record = painting(BarState::Idle);
    const auto thumb = std::ranges::find_if(
        record.fills, [](const RecordedFill& fill) noexcept {
          return fill.area.size == THUMB;
        });
    return thumb == record.fills.end()
               ? sf::Vector2i()
               : thumb->area.position + sf::Vector2i(THUMB.x / 2, THUMB.y / 2);
  }

 private:
  int value_ = MUSIC_DEFAULT;
  int writes_ = 0;
  bool muted_ = false;
  Slider slider_{PERCENT,
                 ValueBinding<int>{.read = [this] { return value_; },
                                   .write =
                                       [this](int value) {
                                         value_ = value;
                                         ++writes_;
                                       }},
                 [this] { return muted_; }, "%"};
};

}  // namespace

/**
 * Given the slider at 70
 * When its width is read
 * Then it is the 168 pixels of the picture: groove, gap and value
 */
TEST_F(SliderTest, TakesTheWidthOfThePicture) {
  EXPECT_EQ(slider().width(), FIRST_ROW_SLIDER.size.x);
}

/**
 * Given the slider at 70
 * When Right is pressed
 * Then it is at 75
 */
TEST_F(SliderTest, RightRaisesItByAStep) {
  EXPECT_TRUE(press(Key::Right));
  EXPECT_EQ(value(), MUSIC_DEFAULT + VOLUME_STEP_OF_THE_OPTIONS);
}

/**
 * Given the slider at 0
 * When Left is pressed
 * Then the key is used, and nothing is written
 */
TEST_F(SliderTest, LeftAtTheMinimumWritesNothing) {
  setValue(0);
  EXPECT_TRUE(press(Key::Left));
  EXPECT_EQ(value(), 0);
  EXPECT_EQ(writes(), 0);
}

/**
 * Given the slider at 70
 * When End is pressed
 * Then it is at 100
 */
TEST_F(SliderTest, EndGoesToTheMaximum) {
  press(Key::End);
  EXPECT_EQ(value(), MAXIMUM_OF_THE_OPTIONS);
}

/**
 * Given the slider at 70
 * When Home is pressed
 * Then it is at 0
 */
TEST_F(SliderTest, HomeGoesToTheMinimum) {
  press(Key::Home);
  EXPECT_EQ(value(), 0);
}

/**
 * Given the slider at 70
 * When Page Up is pressed
 * Then it is at 80, a page of two steps further
 */
TEST_F(SliderTest, PageUpRaisesItByAPage) {
  press(Key::PageUp);
  EXPECT_EQ(value(), 80);
}

/**
 * Given the slider at 5
 * When Page Down is pressed
 * Then it stops at 0
 */
TEST_F(SliderTest, PageDownStopsAtTheMinimum) {
  setValue(VOLUME_STEP_OF_THE_OPTIONS);
  press(Key::PageDown);
  EXPECT_EQ(value(), 0);
}

/**
 * Given the slider
 * When Up or Down is pressed
 * Then it leaves them to the menu, which moves between rows
 */
TEST_F(SliderTest, UpAndDownAreLeftToTheMenu) {
  EXPECT_FALSE(press(Key::Up));
  EXPECT_FALSE(press(Key::Down));
}

/**
 * Given the slider at each of its 21 values
 * When the pointer clicks the middle of the thumb as drawn
 * Then the value stays the same
 */
TEST_F(SliderTest, AClickOnTheThumbKeepsItsValue) {
  for (int volume = 0; volume <= MAXIMUM_OF_THE_OPTIONS;
       volume += VOLUME_STEP_OF_THE_OPTIONS) {
    setValue(volume);
    const sf::Vector2i middle = thumbMiddle();
    slider().pointerPressed(middle, FIRST_ROW_SLIDER);
    slider().pointerReleased(middle, FIRST_ROW_SLIDER);
    EXPECT_EQ(value(), volume);
  }
}

/**
 * Given the slider at 70
 * When the pointer presses at the right end of the groove
 * Then it jumps to 100
 */
TEST_F(SliderTest, APressOnTheGrooveJumpsToTheNearestStep) {
  EXPECT_TRUE(slider().pointerPressed(RIGHT_OF_THE_TRACK, FIRST_ROW_SLIDER));
  EXPECT_EQ(value(), MAXIMUM_OF_THE_OPTIONS);
}

/**
 * Given the pointer pressed at the left of the track
 * When it moves to the right of the track, is released, then moves back
 * Then the thumb follows it to 100, and stops following once released
 */
TEST_F(SliderTest, TheThumbFollowsADragUntilTheRelease) {
  slider().pointerPressed(LEFT_OF_THE_TRACK, FIRST_ROW_SLIDER);
  EXPECT_TRUE(slider().holdsInput());
  slider().pointerMoved(RIGHT_OF_THE_TRACK, FIRST_ROW_SLIDER);
  slider().pointerReleased(RIGHT_OF_THE_TRACK, FIRST_ROW_SLIDER);
  slider().pointerMoved(LEFT_OF_THE_TRACK, FIRST_ROW_SLIDER);
  EXPECT_EQ(value(), MAXIMUM_OF_THE_OPTIONS);
  EXPECT_FALSE(slider().holdsInput());
}

/**
 * Given the slider
 * When the pointer presses on its value, right of the groove
 * Then the press is not used, and the menu presses the row
 */
TEST_F(SliderTest, APressOnItsValueIsNotUsed) {
  EXPECT_FALSE(slider().pointerPressed(ON_THE_VALUE, FIRST_ROW_SLIDER));
}

/**
 * Given a drag under way
 * When the slider loses the input, as when the window loses the focus
 * Then it holds the input no more
 */
TEST_F(SliderTest, LosingTheInputEndsADrag) {
  slider().pointerPressed(LEFT_OF_THE_TRACK, FIRST_ROW_SLIDER);
  slider().cancelInput();
  EXPECT_FALSE(slider().holdsInput());
}

/**
 * Given the slider at 70
 * When it is painted
 * Then it writes 70%
 */
TEST_F(SliderTest, ItWritesItsValueAndUnit) {
  const InterfaceCanvasRecord record = painting(BarState::Selected);
  ASSERT_FALSE(record.writings.empty());
  EXPECT_EQ(record.writings.back().text, "70%");
}

/**
 * Given the sound muted
 * When the slider is painted
 * Then it writes OFF instead of its value
 */
TEST_F(SliderTest, AMutedSliderWritesOff) {
  mute();
  const InterfaceCanvasRecord record = painting(BarState::Idle);
  ASSERT_FALSE(record.writings.empty());
  EXPECT_EQ(record.writings.back().text, "OFF");
}
