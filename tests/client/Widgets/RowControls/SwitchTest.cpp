#include <gtest/gtest.h>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include "BarState.hpp"
#include "KeyPress.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "Switch.hpp"
#include "ValueBinding.hpp"

using rtype::client::BarState;
using rtype::client::Switch;
using rtype::client::ValueBinding;
using Key = sf::Keyboard::Key;

namespace {

/** @brief Where the switch of MUTE ALL lies on the picture the team chose,
 * and a pixel of its word ON. */
constexpr sf::IntRect MUTE_ALL_SWITCH({382, 108}, {62, 12});
constexpr sf::Vector2i ON_WORD(382 + 45, 113);

class SwitchTest : public ::testing::Test {
 protected:
  [[nodiscard]] bool on() const { return on_; }
  void turnOn() { on_ = true; }
  [[nodiscard]] int writes() const { return writes_; }
  Switch& muteAll() { return muteAll_; }

 private:
  bool on_ = false;
  int writes_ = 0;
  Switch muteAll_{ValueBinding<bool>{.read = [this] { return on_; },
                                     .write =
                                         [this](bool switchedOn) {
                                           on_ = switchedOn;
                                           ++writes_;
                                         }}};
};

}  // namespace

/**
 * Given the switch off
 * When Right is pressed
 * Then it is on
 */
TEST_F(SwitchTest, RightTurnsItOn) {
  EXPECT_TRUE(muteAll().keyPressed(keyPressOf(Key::Right)));
  EXPECT_TRUE(on());
}

/**
 * Given the switch on
 * When Left is pressed
 * Then it is off
 */
TEST_F(SwitchTest, LeftTurnsItOff) {
  turnOn();
  muteAll().keyPressed(keyPressOf(Key::Left));
  EXPECT_FALSE(on());
}

/**
 * Given the switch on
 * When Right is pressed again
 * Then nothing is written: Right sets on, it never turns over
 */
TEST_F(SwitchTest, RightOnASwitchOnWritesNothing) {
  turnOn();
  muteAll().keyPressed(keyPressOf(Key::Right));
  EXPECT_EQ(writes(), 0);
}

/**
 * Given the switch off
 * When its row is activated, as Enter does
 * Then it is on
 */
TEST_F(SwitchTest, ActivatingItTurnsItOver) {
  muteAll().activate();
  EXPECT_TRUE(on());
}

/**
 * Given the switch off
 * When the pointer presses the word ON
 * Then it is on, and the press is used
 */
TEST_F(SwitchTest, AClickOnAWordSetsIt) {
  EXPECT_TRUE(muteAll().pointerPressed(ON_WORD, MUTE_ALL_SWITCH));
  EXPECT_TRUE(on());
}

/**
 * Given the switch
 * When it is painted
 * Then both words are written, so its state reads without its color
 */
TEST_F(SwitchTest, ItWritesBothWords) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  muteAll().paint(canvas, MUTE_ALL_SWITCH, BarState::Idle);
  EXPECT_NE(writingOf(record, "OFF"), nullptr);
  EXPECT_NE(writingOf(record, "ON"), nullptr);
  EXPECT_EQ(muteAll().width(), MUTE_ALL_SWITCH.size.x);
}
