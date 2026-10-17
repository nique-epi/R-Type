#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "InterfaceConstants.hpp"
#include "KeyPress.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "MenuPainting.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingRowControl.hpp"
#include "ScreenTestConstants.hpp"
#include "TextField.hpp"
#include "TextStyle.hpp"
#include "TimeConstants.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"

using rtype::client::BUTTON_TEXT_OFFSET;
using rtype::client::Menu;
using rtype::client::MenuBuilder;
using rtype::client::PRESS_DURATION;
using rtype::client::PRESSED_BAR_TEXT_COLOR;
using rtype::client::TEXT_LINE_HEIGHT;
using rtype::client::TextField;
using rtype::client::TextFont;
using rtype::engine::Duration;
using ::testing::Contains;
using ::testing::ElementsAre;
using ::testing::Field;
using ::testing::IsEmpty;
using ::testing::Not;

namespace {

constexpr std::size_t FIRST = 0;
constexpr std::size_t SECOND = 1;
constexpr std::size_t LAST = 2;
constexpr sf::Keyboard::Key ENTER = sf::Keyboard::Key::Enter;
constexpr sf::Keyboard::Key ESCAPE = sf::Keyboard::Key::Escape;
constexpr Duration JUST_BEFORE_DUE = PRESS_DURATION - ONE_TICK;
constexpr sf::Vector2i ONE_UNIT_LEFT(-1, 0);
constexpr sf::Vector2i ONE_PIXEL_LOWER(0, 1);
constexpr sf::Vector2i INSIDE_A_BAR(4, 4);

/** @returns The point of the playfield at the top-left corner of the bar of a
 * menu whose line reads so, read from what the menu paints. */
sf::Vector2i barCornerOf(const Menu& menu, const std::string& line) {
  const InterfaceCanvasRecord record = paintingOf(menu);
  const RecordedWriting* writing = writingOf(record, line);
  return writing == nullptr
             ? sf::Vector2i()
             : (writing->anchor - BUTTON_TEXT_OFFSET) * INTERFACE_PIXEL;
}

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
   * labelled so. */
  sf::Vector2i barCorner(const std::string& label) {
    return barCornerOf(menu_, label);
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

  const InterfaceCanvasRecord record = paintingOf(menu());

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

namespace {

constexpr std::size_t FIELD = 0;
constexpr std::size_t BUTTON_AFTER_THE_FIELD = 1;
constexpr std::size_t FOUR_BYTES = 4;

/** @brief Width of every character of the regular font, in pixels, read from
 * its file. */
constexpr int CHARACTER_WIDTH = 8;

/**
 * @brief The caret the team chose: one pixel wide, and taller than a line of
 * text by a pixel above and below it.
 */
constexpr sf::Vector2i THIN_CARET_SIZE(1, 10);
constexpr sf::Vector2i ONE_PIXEL_HIGHER(0, -1);

/** @brief Characters of the texts the field holds in the tests. */
constexpr int CHARACTERS_OF_AB = 2;
constexpr int CHARACTERS_OF_AB_AND_E_ACUTE = 3;

/** @brief The letter e with an acute accent, one character of two bytes, and
 * the text AB followed by it, in UTF-8. */
constexpr char32_t E_ACUTE = U'\xE9';
constexpr const char* AB_AND_E_ACUTE = "AB\xC3\xA9";

/** @returns Where the caret lies after a text of so many characters written
 * at an anchor. */
sf::IntRect caretAfter(sf::Vector2i textAnchor, int characters) {
  return {textAnchor + sf::Vector2i(characters * CHARACTER_WIDTH, 0) +
              ONE_PIXEL_HIGHER,
          THIN_CARET_SIZE};
}

/**
 * @brief A form whose first bar is a field of four bytes, NAME, starting with
 * AB, then a button, OK; their actions write their label in a journal.
 */
class MenuWithAFieldTest : public ::testing::Test {
 protected:
  Menu& menu() { return menu_; }

  [[nodiscard]] const std::vector<std::string>& activated() const {
    return activated_;
  }

  void send(const sf::Event& event) {
    menu_.handleEvent(event, samePlayfieldPoint);
  }

  /** @returns What the menu paints. */
  [[nodiscard]] InterfaceCanvasRecord painting() const {
    return paintingOf(menu_);
  }

  /** @returns The point of the playfield one pixel inside the line written
   * with this text, read from what the menu paints. */
  [[nodiscard]] sf::Vector2i pointOn(const std::string& text) const {
    const InterfaceCanvasRecord record = painting();
    const RecordedWriting* writing = writingOf(record, text);
    return writing == nullptr
               ? sf::Vector2i()
               : (writing->anchor * INTERFACE_PIXEL) + INSIDE_A_BAR;
  }

 private:
  std::vector<std::string> activated_;
  Menu menu_ =
      MenuBuilder(MenuBuilder::Frame::Form)
          .textField(
              {.label = "NAME", .description = "> TYPE A NAME"},
              TextField("AB", FOUR_BYTES, TextField::Characters::Printable),
              [this] { activated_.emplace_back("NAME"); })
          .button({.label = "OK"}, [this] { activated_.emplace_back("OK"); })
          .build();
};

}  // namespace

/**
 * Given a form whose field, holding AB, is selected
 * When C is typed
 * Then the field holds ABC
 */
TEST_F(MenuWithAFieldTest, ACharacterTypedGoesToTheSelectedField) {
  send(typedCharacter(U'C'));

  EXPECT_EQ(menu().fieldText(FIELD), "ABC");
}

/**
 * Given a form whose button after the field is selected
 * When C is typed
 * Then the field still holds AB
 */
TEST_F(MenuWithAFieldTest, ACharacterTypedOnAButtonGoesNowhere) {
  send(keyPress(sf::Keyboard::Key::Down));

  send(typedCharacter(U'C'));

  EXPECT_EQ(menu().fieldText(FIELD), "AB");
}

/**
 * Given a form whose field, holding AB, is selected
 * When Backspace is pressed
 * Then the field holds A
 */
TEST_F(MenuWithAFieldTest, BackspaceErasesFromTheSelectedField) {
  send(keyPress(sf::Keyboard::Key::Backspace));

  EXPECT_EQ(menu().fieldText(FIELD), "A");
}

/**
 * Given a form whose field is selected and pressed with Enter
 * When C is typed before the press ends
 * Then the field still holds AB: every input waits while a bar is pressed
 */
TEST_F(MenuWithAFieldTest, TypingWaitsWhileTheFieldIsPressed) {
  send(keyPress(ENTER));

  send(typedCharacter(U'C'));

  EXPECT_EQ(menu().fieldText(FIELD), "AB");
}

/**
 * Given a form whose field is selected
 * When Enter is pressed and released, and the press lasts
 * Then the action of the field runs
 */
TEST_F(MenuWithAFieldTest, EnterOnAFieldRunsItsAction) {
  send(keyPress(ENTER));
  send(keyRelease(ENTER));

  menu().advance(PRESS_DURATION);

  EXPECT_THAT(activated(), ElementsAre("NAME"));
}

/**
 * Given a form whose button after the field is selected
 * When the left button is pressed and released on the text of the field
 * Then the field is selected
 */
TEST_F(MenuWithAFieldTest, AClickOnAFieldSelectsIt) {
  send(keyPress(sf::Keyboard::Key::Down));
  const sf::Vector2i field = pointOn("AB");

  send(leftPress(field));
  send(leftRelease(field));

  EXPECT_EQ(menu().selectedButton(), FIELD);
}

/**
 * Given a form whose button after the field is selected
 * When the field is clicked, then a press duration goes by
 * Then no action runs
 */
TEST_F(MenuWithAFieldTest, AClickOnAFieldRunsNothing) {
  send(keyPress(sf::Keyboard::Key::Down));
  const sf::Vector2i field = pointOn("AB");

  send(leftPress(field));
  send(leftRelease(field));
  menu().advance(PRESS_DURATION);

  EXPECT_THAT(activated(), IsEmpty());
}

/**
 * Given a form whose button after the field is selected
 * When the left button is pressed and released on the label of the field
 * Then the field is selected, as a click in its box would
 */
TEST_F(MenuWithAFieldTest, AClickOnTheLabelOfAFieldSelectsIt) {
  send(keyPress(sf::Keyboard::Key::Down));
  const sf::Vector2i label = pointOn("NAME");

  send(leftPress(label));
  send(leftRelease(label));

  EXPECT_EQ(menu().selectedButton(), FIELD);
}

/**
 * Given a form whose field is selected
 * When the pointer moves over the button after it
 * Then the field stays selected: a form selects on a click only
 */
TEST_F(MenuWithAFieldTest, ThePointerPassingOverAButtonKeepsTheField) {
  const sf::Vector2i button = pointOn("OK");

  send(pointerMove(button));

  EXPECT_EQ(menu().selectedButton(), FIELD);
}

/**
 * Given a form whose field is selected
 * When the left button is pressed and released on the button after it, and
 * the press lasts
 * Then the action of that button runs
 */
TEST_F(MenuWithAFieldTest, AClickOnAButtonOfAFormRunsIt) {
  const sf::Vector2i button = pointOn("OK");

  send(leftPress(button));
  send(leftRelease(button));
  menu().advance(PRESS_DURATION);

  EXPECT_THAT(activated(), ElementsAre("OK"));
}

/**
 * Given a form whose field, holding AB, is selected
 * When the menu is painted
 * Then a caret one pixel wide stands right after the text
 */
TEST_F(MenuWithAFieldTest, TheSelectedFieldShowsAThinCaretAfterItsText) {
  const InterfaceCanvasRecord record = painting();

  const RecordedWriting* text = writingOf(record, "AB");
  ASSERT_NE(text, nullptr);
  EXPECT_THAT(record.fills,
              Contains(Field(&RecordedFill::area,
                             caretAfter(text->anchor, CHARACTERS_OF_AB))));
}

/**
 * Given a form whose button after the field is selected
 * When the menu is painted
 * Then no caret is painted
 */
TEST_F(MenuWithAFieldTest, AFieldNotSelectedShowsNoCaret) {
  send(keyPress(sf::Keyboard::Key::Down));

  EXPECT_THAT(painting().fills,
              Not(Contains(Field(&RecordedFill::area,
                                 Field(&sf::IntRect::size, THIN_CARET_SIZE)))));
}

/**
 * Given a form whose field, holding AB, is selected
 * When an e with an acute accent, one character of two bytes, is typed
 * Then the caret stands three characters after the start of the text, not
 * four
 */
TEST_F(MenuWithAFieldTest, TheCaretCountsCharactersNotBytes) {
  send(typedCharacter(E_ACUTE));

  const InterfaceCanvasRecord record = painting();

  const RecordedWriting* text = writingOf(record, AB_AND_E_ACUTE);
  ASSERT_NE(text, nullptr);
  EXPECT_THAT(
      record.fills,
      Contains(Field(&RecordedFill::area,
                     caretAfter(text->anchor, CHARACTERS_OF_AB_AND_E_ACUTE))));
}

/**
 * Given a form with a field and a button
 * When the text of the button is read
 * Then it has none
 */
TEST_F(MenuWithAFieldTest, AButtonHasNoText) {
  EXPECT_EQ(menu().fieldText(BUTTON_AFTER_THE_FIELD), "");
}

/**
 * Given a form whose field is selected, and its description written
 * When a status line is set and the menu painted
 * Then it is written in line with the description, below it and clear of it
 */
TEST_F(MenuWithAFieldTest, TheStatusLineLiesBelowTheDescription) {
  menu().setStatus("WAIT");

  const InterfaceCanvasRecord record = painting();

  const RecordedWriting* description = writingOf(record, "> TYPE A NAME");
  const RecordedWriting* status = writingOf(record, "WAIT");
  ASSERT_NE(description, nullptr);
  ASSERT_NE(status, nullptr);
  EXPECT_EQ(status->anchor.x, description->anchor.x);
  EXPECT_GE(status->anchor.y, description->anchor.y + TEXT_LINE_HEIGHT);
}

/**
 * Given a dialog
 * When a status line is set and the dialog painted
 * Then no status line is written: only a form has one
 */
TEST(MenuStatus, ADialogWritesNoStatusLine) {
  Menu dialog = MenuBuilder(MenuBuilder::Frame::Dialog)
                    .button({.label = "OK"}, [] {})
                    .build();
  dialog.setStatus("WAIT");

  const InterfaceCanvasRecord record = paintingOf(dialog);

  EXPECT_EQ(writingOf(record, "WAIT"), nullptr);
}

/**
 * Given a page
 * When a status line is set and the page painted
 * Then no status line is written: only a form has one
 */
TEST(MenuStatus, APageWritesNoStatusLine) {
  Menu page = MenuBuilder(MenuBuilder::Frame::Page)
                  .button({.label = "OK"}, [] {})
                  .build();
  page.setStatus("WAIT");

  const InterfaceCanvasRecord record = paintingOf(page);

  EXPECT_EQ(writingOf(record, "WAIT"), nullptr);
}

namespace {

constexpr std::size_t SECOND_TAB = 1;
constexpr std::size_t LAST_TAB = 2;
constexpr std::size_t LINK_ROW = 1;

/** @brief A tabbed panel of three tabs: a row whose control steps on Left
 * and Right, a row whose control can be activated, then BACK. */
class TabbedPanelMenuTest : public ::testing::Test {
 protected:
  void send(const sf::Event& event) {
    menu_.handleEvent(event, samePlayfieldPoint);
  }

  void tap(sf::Keyboard::Key key,
           sf::Keyboard::Scancode scancode = sf::Keyboard::Scan::Unknown) {
    send(keyPress(key, scancode));
    send(keyRelease(key, scancode));
    menu_.advance(PRESS_DURATION);
  }

  Menu& menu() { return menu_; }
  RowControlRecord& steps() { return steps_; }
  RowControlRecord& link() { return link_; }
  [[nodiscard]] const std::vector<std::size_t>& opened() const {
    return opened_;
  }
  [[nodiscard]] int backs() const { return backs_; }

 private:
  RowControlRecord steps_;
  RowControlRecord link_;
  std::vector<std::size_t> opened_;
  int backs_ = 0;
  Menu menu_ =
      MenuBuilder(MenuBuilder::Frame::TabbedPanel)
          .heading("SETTINGS")
          .tabs({"A", "B", "C"}, 0, {.previous = "Q", .next = "E"},
                [this](std::size_t tab) { opened_.push_back(tab); })
          .row({.label = "STEPS"}, std::make_unique<RecordingRowControl>(
                                       steps_,
                                       std::vector{sf::Keyboard::Key::Left,
                                                   sf::Keyboard::Key::Right},
                                       false))
          .row({.label = "LINK"},
               std::make_unique<RecordingRowControl>(
                   link_, std::vector<sf::Keyboard::Key>{}, true))
          .cancelButton({.label = "BACK"}, [this] { ++backs_; })
          .build();
};

}  // namespace

/**
 * Given the row that steps selected
 * When Right is tapped
 * Then its control gets Right, and the selection stays on it
 */
TEST_F(TabbedPanelMenuTest, RightGoesToTheControlOfTheSelectedRow) {
  tap(sf::Keyboard::Key::Right);
  EXPECT_EQ(steps().keys, std::vector{sf::Keyboard::Key::Right});
  EXPECT_EQ(menu().selectedButton(), 0U);
}

/**
 * Given the link row selected, whose control does not use Right
 * When Right is tapped
 * Then the selection stays on it: in a tabbed panel, Right steps a value
 */
TEST_F(TabbedPanelMenuTest, ARightLeftUnusedDoesNothing) {
  tap(sf::Keyboard::Key::Down);
  tap(sf::Keyboard::Key::Right);
  EXPECT_EQ(menu().selectedButton(), LINK_ROW);
}

/**
 * Given the row that steps selected, which cannot be activated
 * When Enter is tapped and the press time passes
 * Then nothing is activated
 */
TEST_F(TabbedPanelMenuTest, EnterOnARowThatCannotBeActivatedDoesNothing) {
  tap(sf::Keyboard::Key::Enter);
  EXPECT_EQ(steps().activations, 0);
}

/**
 * Given the link row selected
 * When Enter is tapped and the press time passes
 * Then its control is activated once
 */
TEST_F(TabbedPanelMenuTest, EnterActivatesTheControlOfALinkRow) {
  tap(sf::Keyboard::Key::Down);
  tap(sf::Keyboard::Key::Enter);
  EXPECT_EQ(link().activations, 1);
}

/**
 * Given the first tab open
 * When the key at the place of E is tapped
 * Then the second tab is asked for
 */
TEST_F(TabbedPanelMenuTest, TheKeyAtThePlaceOfEOpensTheNextTab) {
  tap(sf::Keyboard::Key::E, sf::Keyboard::Scan::E);
  EXPECT_EQ(opened(), std::vector{SECOND_TAB});
}

/**
 * Given an AZERTY keyboard, whose key at the place of Q prints A
 * When that key is tapped
 * Then the last tab is asked for: the key is read by its place
 */
TEST_F(TabbedPanelMenuTest, TheKeyAtThePlaceOfQOpensThePreviousTab) {
  tap(sf::Keyboard::Key::A, sf::Keyboard::Scan::Q);
  EXPECT_EQ(opened(), std::vector{LAST_TAB});
}

/**
 * Given an AZERTY keyboard, whose key printing Q sits at the place of A
 * When that key is tapped
 * Then no tab is asked for: the character a key prints does not count
 */
TEST_F(TabbedPanelMenuTest, TheKeyPrintingQElsewhereOpensNoTab) {
  tap(sf::Keyboard::Key::Q, sf::Keyboard::Scan::A);
  EXPECT_TRUE(opened().empty());
}

/**
 * Given the control of the selected row holding the input
 * When Down, the key at the place of E, then Escape are tapped
 * Then all three reach the control, the selection stays, no tab opens and
 * BACK is not activated
 */
TEST_F(TabbedPanelMenuTest, AControlHoldingTheInputGetsEveryKey) {
  steps().holding = true;
  tap(sf::Keyboard::Key::Down);
  tap(sf::Keyboard::Key::E, sf::Keyboard::Scan::E);
  tap(sf::Keyboard::Key::Escape);
  EXPECT_EQ(steps().keys,
            (std::vector{sf::Keyboard::Key::Down, sf::Keyboard::Key::E,
                         sf::Keyboard::Key::Escape}));
  EXPECT_EQ(menu().selectedButton(), 0U);
  EXPECT_TRUE(opened().empty());
  EXPECT_EQ(backs(), 0);
}

/**
 * Given the control of the selected row holding the input
 * When the window loses the focus
 * Then the control is told to drop it
 */
TEST_F(TabbedPanelMenuTest, LosingTheFocusDropsTheInputHeld) {
  steps().holding = true;
  send(sf::Event::FocusLost{});
  EXPECT_EQ(steps().cancels, 1);
  EXPECT_FALSE(steps().holding);
}

/**
 * Given the first tab open
 * When the pointer clicks the label of the second tab
 * Then the second tab is asked for
 */
TEST_F(TabbedPanelMenuTest, AClickOnATabOpensIt) {
  const InterfaceCanvasRecord painting = paintingOf(menu());
  const RecordedWriting* label = writingOf(painting, "B");
  ASSERT_NE(label, nullptr);
  send(leftPress(label->anchor * INTERFACE_PIXEL));
  EXPECT_EQ(opened(), std::vector{SECOND_TAB});
}

/**
 * Given a status set on the panel
 * When it is painted
 * Then the status is stamped at (270, 36), the end of the rule on the
 * picture, in small print
 */
TEST_F(TabbedPanelMenuTest, TheStatusIsStampedAtTheEndOfTheRule) {
  menu().setStatus("SAVED");
  const InterfaceCanvasRecord painting = paintingOf(menu());
  const RecordedWriting* stamp = writingOf(painting, "SAVED");
  ASSERT_NE(stamp, nullptr);
  EXPECT_EQ(stamp->anchor, sf::Vector2i(270, 36));
  EXPECT_EQ(stamp->style.font, TextFont::Small);
}
