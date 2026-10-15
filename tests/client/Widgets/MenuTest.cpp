#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <chrono>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>
#include "BackgroundConstants.hpp"
#include "KeyPress.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "TimeConstants.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"

using rtype::client::BACKGROUND_PIXEL_SIZE;
using rtype::client::BUTTON_TEXT_OFFSET;
using rtype::client::Menu;
using rtype::client::MenuBuilder;
using rtype::client::PRESS_DURATION;
using rtype::client::PRESSED_BAR_TEXT_COLOR;
using rtype::engine::Duration;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

namespace {

constexpr std::size_t FIRST = 0;
constexpr std::size_t SECOND = 1;
constexpr std::size_t LAST = 2;
constexpr sf::Keyboard::Key ENTER = sf::Keyboard::Key::Enter;
constexpr sf::Keyboard::Key ESCAPE = sf::Keyboard::Key::Escape;
constexpr Duration ONE_TICK = std::chrono::nanoseconds(1);
constexpr Duration JUST_BEFORE_DUE = PRESS_DURATION - ONE_TICK;
constexpr int INTERFACE_PIXEL = static_cast<int>(BACKGROUND_PIXEL_SIZE);
constexpr sf::Vector2i ONE_UNIT_LEFT(-1, 0);
constexpr sf::Vector2i ONE_PIXEL_LOWER(0, 1);
constexpr sf::Vector2i INSIDE_A_BAR(4, 4);

/** @brief A page of three buttons, A, B and C, C being the cancel button;
 * their actions write their label in a journal. */
class MenuTest : public ::testing::Test {
 protected:
  Menu& menu() { return menu_; }

  [[nodiscard]] const std::vector<std::string>& activated() const {
    return activated_;
  }

  void send(const sf::Event& event) {
    menu_.handleEvent(event, samePlayfieldPoint);
  }

  void tap(sf::Keyboard::Key key) {
    send(keyPress(key));
    send(keyRelease(key));
  }

  /** @returns The point of the playfield at the top-left corner of the bar
   * labelled so, read from what the menu paints. */
  sf::Vector2i barCorner(const std::string& label) {
    InterfaceCanvasRecord record;
    RecordingInterfaceCanvas canvas(record);
    menu_.paint(canvas);
    const RecordedWriting* writing = writingOf(record, label);
    return writing == nullptr
               ? sf::Vector2i()
               : (writing->anchor - BUTTON_TEXT_OFFSET) * INTERFACE_PIXEL;
  }

 private:
  std::vector<std::string> activated_;
  Menu menu_ =
      MenuBuilder(MenuBuilder::Frame::Page)
          .button({.label = "A"}, [this] { activated_.emplace_back("A"); })
          .button({.label = "B"}, [this] { activated_.emplace_back("B"); })
          .cancelButton({.label = "C"},
                        [this] { activated_.emplace_back("C"); })
          .build();
};

}  // namespace

/**
 * Given a page of three buttons just built
 * When nothing happened yet
 * Then its first button is selected
 */
TEST_F(MenuTest, SelectsItsFirstButtonAtFirst) {
  EXPECT_EQ(menu().selectedButton(), FIRST);
}

/**
 * Given the first button selected
 * When the down arrow is pressed
 * Then the second button is selected
 */
TEST_F(MenuTest, DownArrowSelectsTheNextButton) {
  send(keyPress(sf::Keyboard::Key::Down));

  EXPECT_EQ(menu().selectedButton(), SECOND);
}

/**
 * Given the first button selected
 * When the up arrow is pressed
 * Then the selection wraps around to the last button
 */
TEST_F(MenuTest, UpArrowOnTheFirstButtonWrapsToTheLast) {
  send(keyPress(sf::Keyboard::Key::Up));

  EXPECT_EQ(menu().selectedButton(), LAST);
}

/**
 * Given the first button selected
 * When Tab is pressed
 * Then the second button is selected
 */
TEST_F(MenuTest, TabSelectsTheNextButton) {
  send(keyPress(sf::Keyboard::Key::Tab));

  EXPECT_EQ(menu().selectedButton(), SECOND);
}

/**
 * Given the first button selected
 * When Shift+Tab is pressed
 * Then the selection goes back to the last button
 */
TEST_F(MenuTest, ShiftTabSelectsThePreviousButton) {
  send(shiftedKeyPress(sf::Keyboard::Key::Tab));

  EXPECT_EQ(menu().selectedButton(), LAST);
}

/**
 * Given the first button selected and the second one disabled
 * When the down arrow is pressed
 * Then the third button is selected
 */
TEST_F(MenuTest, ArrowsSkipADisabledButton) {
  menu().setEnabled(SECOND, false);

  send(keyPress(sf::Keyboard::Key::Down));

  EXPECT_EQ(menu().selectedButton(), LAST);
}

/**
 * Given the first button selected
 * When it is disabled
 * Then the next enabled button is selected
 */
TEST_F(MenuTest, DisablingTheSelectedButtonSelectsTheNextOne) {
  menu().setEnabled(FIRST, false);

  EXPECT_EQ(menu().selectedButton(), SECOND);
}

/**
 * Given a page of three buttons
 * When all of them are disabled
 * Then no button is selected
 */
TEST_F(MenuTest, NoButtonIsSelectedWhenNoneIsEnabled) {
  menu().setEnabled(FIRST, false);
  menu().setEnabled(SECOND, false);

  menu().setEnabled(LAST, false);

  EXPECT_EQ(menu().selectedButton(), std::nullopt);
}

/**
 * Given the first button selected
 * When Enter is pressed and released, and the press has lasted
 * PRESS_DURATION
 * Then the first button is activated, once
 */
TEST_F(MenuTest, EnterActivatesTheSelectedButtonOncePressedLongEnough) {
  tap(ENTER);

  menu().advance(PRESS_DURATION);

  EXPECT_THAT(activated(), ElementsAre("A"));
}

/**
 * Given Enter pressed and released on the first button
 * When a little less than PRESS_DURATION has passed
 * Then nothing is activated yet
 */
TEST_F(MenuTest, AQuickPressWaitsForThePressDuration) {
  tap(ENTER);

  menu().advance(JUST_BEFORE_DUE);

  EXPECT_THAT(activated(), IsEmpty());
}

/**
 * Given Enter pressed and released, a little less than PRESS_DURATION ago
 * When the rest of PRESS_DURATION passes
 * Then the first button is activated
 */
TEST_F(MenuTest, AQuickPressActivatesOnceThePressDurationHasPassed) {
  tap(ENTER);
  menu().advance(JUST_BEFORE_DUE);

  menu().advance(ONE_TICK);

  EXPECT_THAT(activated(), ElementsAre("A"));
}

/**
 * Given Enter held for PRESS_DURATION
 * When it is released
 * Then the first button is activated at once
 */
TEST_F(MenuTest, ALongPressActivatesOnRelease) {
  send(keyPress(ENTER));
  menu().advance(PRESS_DURATION);

  send(keyRelease(ENTER));

  EXPECT_THAT(activated(), ElementsAre("A"));
}

/**
 * Given Enter held, its press repeated by the system
 * When it is released and PRESS_DURATION passes
 * Then the first button is activated once only
 */
TEST_F(MenuTest, RepeatedPressesOfAHeldKeyActivateOnce) {
  send(keyPress(ENTER));
  send(keyPress(ENTER));
  send(keyPress(ENTER));

  send(keyRelease(ENTER));
  menu().advance(PRESS_DURATION);

  EXPECT_THAT(activated(), ElementsAre("A"));
}

/**
 * Given a menu that never saw Enter pressed, as when Enter opened its screen
 * When Enter is released and PRESS_DURATION passes
 * Then nothing is activated
 */
TEST_F(MenuTest, ReleaseOfAKeyPressedOnAnotherScreenDoesNothing) {
  send(keyRelease(ENTER));

  menu().advance(PRESS_DURATION);

  EXPECT_THAT(activated(), IsEmpty());
}

/**
 * Given the first button selected
 * When Escape is pressed and released
 * Then the cancel button is activated at once
 */
TEST_F(MenuTest, EscapeActivatesTheCancelButtonAtOnce) {
  tap(ESCAPE);

  EXPECT_THAT(activated(), ElementsAre("C"));
}

/**
 * Given the first button selected
 * When Escape is pressed and released
 * Then the cancel button is selected, as it was activated
 */
TEST_F(MenuTest, EscapeSelectsTheCancelButton) {
  tap(ESCAPE);

  EXPECT_EQ(menu().selectedButton(), LAST);
}

/**
 * Given a menu that never saw Escape pressed
 * When Escape is released
 * Then nothing is activated
 */
TEST_F(MenuTest, EscapeReleasedWithoutItsPressDoesNothing) {
  send(keyRelease(ESCAPE));

  EXPECT_THAT(activated(), IsEmpty());
}

/**
 * Given a page whose only button is not a cancel button
 * When Escape is pressed and released
 * Then nothing is activated
 */
TEST(Menu, EscapeWithoutCancelButtonDoesNothing) {
  bool activated = false;
  Menu menu = MenuBuilder(MenuBuilder::Frame::Page)
                  .button({.label = "A"}, [&activated] { activated = true; })
                  .build();

  menu.handleEvent(keyPress(ESCAPE), samePlayfieldPoint);
  menu.handleEvent(keyRelease(ESCAPE), samePlayfieldPoint);

  EXPECT_FALSE(activated);
}

/**
 * Given the first button selected
 * When the pointer moves over the second button
 * Then the second button is selected
 */
TEST_F(MenuTest, PointerOverAButtonSelectsIt) {
  const sf::Vector2i second = barCorner("B");

  send(pointerMove(second + INSIDE_A_BAR));

  EXPECT_EQ(menu().selectedButton(), SECOND);
}

/**
 * Given the second button disabled
 * When the pointer moves over it
 * Then the first button stays selected
 */
TEST_F(MenuTest, PointerOverADisabledButtonKeepsTheSelection) {
  const sf::Vector2i second = barCorner("B");
  menu().setEnabled(SECOND, false);

  send(pointerMove(second + INSIDE_A_BAR));

  EXPECT_EQ(menu().selectedButton(), FIRST);
}

/**
 * Given the first button selected
 * When the pointer moves to the first column of pixels of the second button
 * Then the second button is selected
 */
TEST_F(MenuTest, PointerOnTheLeftEdgeOfAButtonSelectsIt) {
  const sf::Vector2i second = barCorner("B");

  send(pointerMove(second));

  EXPECT_EQ(menu().selectedButton(), SECOND);
}

/**
 * Given the first button selected
 * When the pointer moves one logical unit left of the second button
 * Then the first button stays selected
 */
TEST_F(MenuTest, PointerJustLeftOfAButtonSelectsNothing) {
  const sf::Vector2i second = barCorner("B");

  send(pointerMove(second + ONE_UNIT_LEFT));

  EXPECT_EQ(menu().selectedButton(), FIRST);
}

/**
 * Given the pointer over the second button
 * When the left button is pressed and released there, and PRESS_DURATION
 * passes
 * Then the second button is activated
 */
TEST_F(MenuTest, ClickActivatesTheButtonUnderThePointer) {
  const sf::Vector2i second = barCorner("B") + INSIDE_A_BAR;
  send(leftPress(second));
  send(leftRelease(second));

  menu().advance(PRESS_DURATION);

  EXPECT_THAT(activated(), ElementsAre("B"));
}

/**
 * Given the left button pressed over the second button
 * When it is released over the third, and PRESS_DURATION passes
 * Then nothing is activated
 */
TEST_F(MenuTest, ReleasingThePointerOffTheButtonDropsThePress) {
  send(leftPress(barCorner("B") + INSIDE_A_BAR));
  send(leftRelease(barCorner("C") + INSIDE_A_BAR));

  menu().advance(PRESS_DURATION);

  EXPECT_THAT(activated(), IsEmpty());
}

/**
 * Given Enter held on the first button
 * When the window loses the focus, then Enter is released
 * Then nothing is activated
 */
TEST_F(MenuTest, LosingTheFocusDropsAHeldPress) {
  send(keyPress(ENTER));
  send(sf::Event{sf::Event::FocusLost{}});

  send(keyRelease(ENTER));
  menu().advance(PRESS_DURATION);

  EXPECT_THAT(activated(), IsEmpty());
}

/**
 * Given Enter held on the first button
 * When the menu is painted
 * Then its label is written in the pressed color, one pixel lower than at
 * rest, as on the mockup
 */
TEST_F(MenuTest, APressedButtonIsPaintedPressed) {
  const sf::Vector2i atRest = barCorner("A") / INTERFACE_PIXEL;
  send(keyPress(ENTER));
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);

  menu().paint(canvas);

  const RecordedWriting* label = writingOf(record, "A");
  ASSERT_NE(label, nullptr);
  EXPECT_EQ(label->style.color, PRESSED_BAR_TEXT_COLOR);
  EXPECT_EQ(label->anchor, atRest + BUTTON_TEXT_OFFSET + ONE_PIXEL_LOWER);
}

/**
 * Given a page without any button
 * When the arrows, Enter, Escape and a click are used
 * Then no button is selected, and nothing breaks
 */
TEST(Menu, AMenuWithoutButtonsIgnoresEveryInput) {
  Menu menu = MenuBuilder(MenuBuilder::Frame::Page).build();

  menu.handleEvent(keyPress(sf::Keyboard::Key::Down), samePlayfieldPoint);
  menu.handleEvent(keyPress(ENTER), samePlayfieldPoint);
  menu.handleEvent(keyRelease(ENTER), samePlayfieldPoint);
  menu.handleEvent(keyPress(ESCAPE), samePlayfieldPoint);
  menu.handleEvent(keyRelease(ESCAPE), samePlayfieldPoint);
  menu.handleEvent(leftPress({}), samePlayfieldPoint);
  menu.advance(PRESS_DURATION);

  EXPECT_EQ(menu.selectedButton(), std::nullopt);
}
