#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>
#include "InterfaceConstants.hpp"
#include "KeyPress.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "MenuFixture.hpp"
#include "MenuFixtures.hpp"
#include "MenuPainting.hpp"
#include "MenuTestConstants.hpp"
#include "MenuWithAFieldFixture.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingRowControl.hpp"
#include "RecordingSoundPlayer.hpp"
#include "ScreenTestConstants.hpp"
#include "SoundConstants.hpp"
#include "TabbedPanelMenuFixture.hpp"
#include "TextStyle.hpp"
#include "TimeConstants.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"

using rtype::client::BUTTON_TEXT_OFFSET;
using rtype::client::CLICK_SOUND_ID;
using rtype::client::Menu;
using rtype::client::MenuBuilder;
using rtype::client::PRESS_DURATION;
using rtype::client::PRESSED_BAR_TEXT_COLOR;
using rtype::client::TEXT_LINE_HEIGHT;
using rtype::client::TextFont;
using rtype::client::testing::AB_AND_E_ACUTE;
using rtype::client::testing::BUTTON_AFTER_THE_FIELD;
using rtype::client::testing::CHARACTERS_OF_AB;
using rtype::client::testing::CHARACTERS_OF_AB_AND_E_ACUTE;
using rtype::client::testing::E_ACUTE;
using rtype::client::testing::ENTER;
using rtype::client::testing::ESCAPE;
using rtype::client::testing::FIELD;
using rtype::client::testing::FIRST;
using rtype::client::testing::INSIDE_A_BAR;
using rtype::client::testing::JUST_BEFORE_DUE;
using rtype::client::testing::LAST;
using rtype::client::testing::LAST_TAB;
using rtype::client::testing::LINK_ROW;
using rtype::client::testing::ONE_PIXEL_LOWER;
using rtype::client::testing::ONE_UNIT_LEFT;
using rtype::client::testing::SECOND;
using rtype::client::testing::SECOND_TAB;
using rtype::client::testing::THIN_CARET_SIZE;
using rtype::engine::Duration;
using ::testing::Contains;
using ::testing::ElementsAre;
using ::testing::Field;
using ::testing::IsEmpty;
using ::testing::Not;

/**
 * Given a page of three buttons just built
 * When nothing happened yet
 * Then its first button is selected
 */
TEST_F(MenuFixture, SelectsItsFirstButtonAtFirst) {
  EXPECT_EQ(menu().selectedButton(), FIRST);
}

/**
 * Given the first button selected
 * When the down arrow is pressed
 * Then the second button is selected
 */
TEST_F(MenuFixture, DownArrowSelectsTheNextButton) {
  send(keyPress(sf::Keyboard::Key::Down));

  EXPECT_EQ(menu().selectedButton(), SECOND);
}

/**
 * Given the first button selected
 * When the up arrow is pressed
 * Then the selection wraps around to the last button
 */
TEST_F(MenuFixture, UpArrowOnTheFirstButtonWrapsToTheLast) {
  send(keyPress(sf::Keyboard::Key::Up));

  EXPECT_EQ(menu().selectedButton(), LAST);
}

/**
 * Given the first button selected
 * When Tab is pressed
 * Then the second button is selected
 */
TEST_F(MenuFixture, TabSelectsTheNextButton) {
  send(keyPress(sf::Keyboard::Key::Tab));

  EXPECT_EQ(menu().selectedButton(), SECOND);
}

/**
 * Given the first button selected
 * When Shift+Tab is pressed
 * Then the selection goes back to the last button
 */
TEST_F(MenuFixture, ShiftTabSelectsThePreviousButton) {
  send(shiftedKeyPress(sf::Keyboard::Key::Tab));

  EXPECT_EQ(menu().selectedButton(), LAST);
}

/**
 * Given the first button selected and the second one disabled
 * When the down arrow is pressed
 * Then the third button is selected
 */
TEST_F(MenuFixture, ArrowsSkipADisabledButton) {
  menu().setEnabled(SECOND, false);

  send(keyPress(sf::Keyboard::Key::Down));

  EXPECT_EQ(menu().selectedButton(), LAST);
}

/**
 * Given the first button selected
 * When it is disabled
 * Then the next enabled button is selected
 */
TEST_F(MenuFixture, DisablingTheSelectedButtonSelectsTheNextOne) {
  menu().setEnabled(FIRST, false);

  EXPECT_EQ(menu().selectedButton(), SECOND);
}

/**
 * Given a page of three buttons
 * When all of them are disabled
 * Then no button is selected
 */
TEST_F(MenuFixture, NoButtonIsSelectedWhenNoneIsEnabled) {
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
TEST_F(MenuFixture, EnterActivatesTheSelectedButtonOncePressedLongEnough) {
  tap(ENTER);

  menu().advance(PRESS_DURATION);

  EXPECT_THAT(activated(), ElementsAre("A"));
}

/**
 * Given Enter pressed and released on the first button
 * When a little less than PRESS_DURATION has passed
 * Then nothing is activated yet
 */
TEST_F(MenuFixture, AQuickPressWaitsForThePressDuration) {
  tap(ENTER);

  menu().advance(JUST_BEFORE_DUE);

  EXPECT_THAT(activated(), IsEmpty());
}

/**
 * Given Enter pressed and released, a little less than PRESS_DURATION ago
 * When the rest of PRESS_DURATION passes
 * Then the first button is activated
 */
TEST_F(MenuFixture, AQuickPressActivatesOnceThePressDurationHasPassed) {
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
TEST_F(MenuFixture, ALongPressActivatesOnRelease) {
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
TEST_F(MenuFixture, RepeatedPressesOfAHeldKeyActivateOnce) {
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
TEST_F(MenuFixture, ReleaseOfAKeyPressedOnAnotherScreenDoesNothing) {
  send(keyRelease(ENTER));

  menu().advance(PRESS_DURATION);

  EXPECT_THAT(activated(), IsEmpty());
}

/**
 * Given the first button selected
 * When Escape is pressed and released
 * Then the cancel button is activated at once
 */
TEST_F(MenuFixture, EscapeActivatesTheCancelButtonAtOnce) {
  tap(ESCAPE);

  EXPECT_THAT(activated(), ElementsAre("C"));
}

/**
 * Given the first button selected
 * When Escape is pressed and released
 * Then the cancel button is selected, as it was activated
 */
TEST_F(MenuFixture, EscapeSelectsTheCancelButton) {
  tap(ESCAPE);

  EXPECT_EQ(menu().selectedButton(), LAST);
}

/**
 * Given a menu that never saw Escape pressed
 * When Escape is released
 * Then nothing is activated
 */
TEST_F(MenuFixture, EscapeReleasedWithoutItsPressDoesNothing) {
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
TEST_F(MenuFixture, PointerOverAButtonSelectsIt) {
  const sf::Vector2i second = barCorner("B");

  send(pointerMove(second + INSIDE_A_BAR));

  EXPECT_EQ(menu().selectedButton(), SECOND);
}

/**
 * Given the second button disabled
 * When the pointer moves over it
 * Then the first button stays selected
 */
TEST_F(MenuFixture, PointerOverADisabledButtonKeepsTheSelection) {
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
TEST_F(MenuFixture, PointerOnTheLeftEdgeOfAButtonSelectsIt) {
  const sf::Vector2i second = barCorner("B");

  send(pointerMove(second));

  EXPECT_EQ(menu().selectedButton(), SECOND);
}

/**
 * Given the first button selected
 * When the pointer moves one logical unit left of the second button
 * Then the first button stays selected
 */
TEST_F(MenuFixture, PointerJustLeftOfAButtonSelectsNothing) {
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
TEST_F(MenuFixture, ClickActivatesTheButtonUnderThePointer) {
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
TEST_F(MenuFixture, ReleasingThePointerOffTheButtonDropsThePress) {
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
TEST_F(MenuFixture, LosingTheFocusDropsAHeldPress) {
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
TEST_F(MenuFixture, APressedButtonIsPaintedPressed) {
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

/**
 * Given a form whose field, holding AB, is selected
 * When C is typed
 * Then the field holds ABC
 */
TEST_F(MenuWithAFieldFixture, ACharacterTypedGoesToTheSelectedField) {
  send(typedCharacter(U'C'));

  EXPECT_EQ(menu().fieldText(FIELD), "ABC");
}

/**
 * Given a form whose button after the field is selected
 * When C is typed
 * Then the field still holds AB
 */
TEST_F(MenuWithAFieldFixture, ACharacterTypedOnAButtonGoesNowhere) {
  send(keyPress(sf::Keyboard::Key::Down));

  send(typedCharacter(U'C'));

  EXPECT_EQ(menu().fieldText(FIELD), "AB");
}

/**
 * Given a form whose field, holding AB, is selected
 * When Backspace is pressed
 * Then the field holds A
 */
TEST_F(MenuWithAFieldFixture, BackspaceErasesFromTheSelectedField) {
  send(keyPress(sf::Keyboard::Key::Backspace));

  EXPECT_EQ(menu().fieldText(FIELD), "A");
}

/**
 * Given a form whose field is selected and pressed with Enter
 * When C is typed before the press ends
 * Then the field still holds AB: every input waits while a bar is pressed
 */
TEST_F(MenuWithAFieldFixture, TypingWaitsWhileTheFieldIsPressed) {
  send(keyPress(ENTER));

  send(typedCharacter(U'C'));

  EXPECT_EQ(menu().fieldText(FIELD), "AB");
}

/**
 * Given a form whose field is selected
 * When Enter is pressed and released, and the press lasts
 * Then the action of the field runs
 */
TEST_F(MenuWithAFieldFixture, EnterOnAFieldRunsItsAction) {
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
TEST_F(MenuWithAFieldFixture, AClickOnAFieldSelectsIt) {
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
TEST_F(MenuWithAFieldFixture, AClickOnAFieldRunsNothing) {
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
TEST_F(MenuWithAFieldFixture, AClickOnTheLabelOfAFieldSelectsIt) {
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
TEST_F(MenuWithAFieldFixture, ThePointerPassingOverAButtonKeepsTheField) {
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
TEST_F(MenuWithAFieldFixture, AClickOnAButtonOfAFormRunsIt) {
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
TEST_F(MenuWithAFieldFixture, TheSelectedFieldShowsAThinCaretAfterItsText) {
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
TEST_F(MenuWithAFieldFixture, AFieldNotSelectedShowsNoCaret) {
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
TEST_F(MenuWithAFieldFixture, TheCaretCountsCharactersNotBytes) {
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
TEST_F(MenuWithAFieldFixture, AButtonHasNoText) {
  EXPECT_EQ(menu().fieldText(BUTTON_AFTER_THE_FIELD), "");
}

/**
 * Given a form whose field is selected, and its description written
 * When a status line is set and the menu painted
 * Then it is written in line with the description, below it and clear of it
 */
TEST_F(MenuWithAFieldFixture, TheStatusLineLiesBelowTheDescription) {
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

/**
 * Given the row that steps selected
 * When Right is tapped
 * Then its control gets Right, and the selection stays on it
 */
TEST_F(TabbedPanelMenuFixture, RightGoesToTheControlOfTheSelectedRow) {
  tap(sf::Keyboard::Key::Right);
  EXPECT_EQ(steps().keys, std::vector{sf::Keyboard::Key::Right});
  EXPECT_EQ(menu().selectedButton(), 0U);
}

/**
 * Given the link row selected, whose control does not use Right
 * When Right is tapped
 * Then the selection stays on it: in a tabbed panel, Right steps a value
 */
TEST_F(TabbedPanelMenuFixture, ARightLeftUnusedDoesNothing) {
  tap(sf::Keyboard::Key::Down);
  tap(sf::Keyboard::Key::Right);
  EXPECT_EQ(menu().selectedButton(), LINK_ROW);
}

/**
 * Given the row that steps selected, which cannot be activated
 * When Enter is tapped and the press time passes
 * Then nothing is activated
 */
TEST_F(TabbedPanelMenuFixture, EnterOnARowThatCannotBeActivatedDoesNothing) {
  tap(sf::Keyboard::Key::Enter);
  EXPECT_EQ(steps().activations, 0);
}

/**
 * Given the link row selected
 * When Enter is tapped and the press time passes
 * Then its control is activated once
 */
TEST_F(TabbedPanelMenuFixture, EnterActivatesTheControlOfALinkRow) {
  tap(sf::Keyboard::Key::Down);
  tap(sf::Keyboard::Key::Enter);
  EXPECT_EQ(link().activations, 1);
}

/**
 * Given the first tab open
 * When the key at the place of E is tapped
 * Then the second tab is asked for
 */
TEST_F(TabbedPanelMenuFixture, TheKeyAtThePlaceOfEOpensTheNextTab) {
  tap(sf::Keyboard::Key::E, sf::Keyboard::Scan::E);
  EXPECT_EQ(opened(), std::vector{SECOND_TAB});
}

/**
 * Given an AZERTY keyboard, whose key at the place of Q prints A
 * When that key is tapped
 * Then the last tab is asked for: the key is read by its place
 */
TEST_F(TabbedPanelMenuFixture, TheKeyAtThePlaceOfQOpensThePreviousTab) {
  tap(sf::Keyboard::Key::A, sf::Keyboard::Scan::Q);
  EXPECT_EQ(opened(), std::vector{LAST_TAB});
}

/**
 * Given an AZERTY keyboard, whose key printing Q sits at the place of A
 * When that key is tapped
 * Then no tab is asked for: the character a key prints does not count
 */
TEST_F(TabbedPanelMenuFixture, TheKeyPrintingQElsewhereOpensNoTab) {
  tap(sf::Keyboard::Key::Q, sf::Keyboard::Scan::A);
  EXPECT_TRUE(opened().empty());
}

/**
 * Given the control of the selected row holding the input
 * When Down, the key at the place of E, then Escape are tapped
 * Then all three reach the control, the selection stays, no tab opens and
 * BACK is not activated
 */
TEST_F(TabbedPanelMenuFixture, AControlHoldingTheInputGetsEveryKey) {
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
TEST_F(TabbedPanelMenuFixture, LosingTheFocusDropsTheInputHeld) {
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
TEST_F(TabbedPanelMenuFixture, AClickOnATabOpensIt) {
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
TEST_F(TabbedPanelMenuFixture, TheStatusIsStampedAtTheEndOfTheRule) {
  menu().setStatus("SAVED");
  const InterfaceCanvasRecord painting = paintingOf(menu());
  const RecordedWriting* stamp = writingOf(painting, "SAVED");
  ASSERT_NE(stamp, nullptr);
  EXPECT_EQ(stamp->anchor, sf::Vector2i(270, 36));
  EXPECT_EQ(stamp->style.font, TextFont::Small);
}

/**
 * Given a page of three buttons that has a sound player
 * When Enter activates its first button
 * Then the click plays once
 */
TEST_F(MenuFixture, ABarActivatedPlaysTheClick) {
  tap(ENTER);

  menu().advance(PRESS_DURATION);

  EXPECT_THAT(sounds().played(), ElementsAre(CLICK_SOUND_ID));
}

/**
 * Given Enter pressed and released on the first button
 * When a little less than PRESS_DURATION has passed
 * Then no click plays yet: it comes with the activation
 */
TEST_F(MenuFixture, APressNotDueYetPlaysNoClick) {
  tap(ENTER);

  menu().advance(JUST_BEFORE_DUE);

  EXPECT_THAT(sounds().played(), IsEmpty());
}

/**
 * Given a page whose last button is the cancel button
 * When Escape is tapped
 * Then the cancel button is activated with the click
 */
TEST_F(MenuFixture, EscapeActivatesTheCancelButtonWithTheClick) {
  tap(ESCAPE);

  EXPECT_THAT(sounds().played(), ElementsAre(CLICK_SOUND_ID));
}

/**
 * Given a form whose field is selected
 * When Enter runs the action of the field
 * Then no click plays: a field is typed into, not pressed
 */
TEST_F(MenuWithAFieldFixture, EnterOnAFieldPlaysNoClick) {
  send(keyPress(ENTER));
  send(keyRelease(ENTER));

  menu().advance(PRESS_DURATION);

  EXPECT_THAT(sounds().played(), IsEmpty());
}

/**
 * Given the first tab of a tabbed panel open
 * When the key at the place of E asks for the next tab
 * Then the click plays once
 */
TEST_F(TabbedPanelMenuFixture, AskingForATabPlaysTheClick) {
  tap(sf::Keyboard::Key::E, sf::Keyboard::Scan::E);

  EXPECT_THAT(sounds().played(), ElementsAre(CLICK_SOUND_ID));
}

/**
 * Given the row that steps selected
 * When Right steps its control
 * Then no click plays: a control sets a value, as a slider does
 */
TEST_F(TabbedPanelMenuFixture, AControlSteppingPlaysNoClick) {
  tap(sf::Keyboard::Key::Right);

  EXPECT_THAT(sounds().played(), IsEmpty());
}

/**
 * Given the row whose control can be activated selected
 * When Enter activates it
 * Then no click plays: only a bar sounds when it is activated
 */
TEST_F(TabbedPanelMenuFixture, ARowActivatedPlaysNoClick) {
  tap(sf::Keyboard::Key::Down);

  tap(sf::Keyboard::Key::Enter);

  EXPECT_THAT(sounds().played(), IsEmpty());
}
