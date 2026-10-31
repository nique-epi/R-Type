#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include "BlankRenderTarget.hpp"
#include "DialogScreenFixture.hpp"
#include "DialogScreenTestConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "WidgetConstants.hpp"

using rtype::client::PRESS_DURATION;
using rtype::client::testing::A_DIALOG_HEADING;

/**
 * Given a dialog screen just built
 * When nothing else happened
 * Then its picture already holds its menu
 */
TEST_F(DialogScreenFixture, PaintsItsMenuOnceBuilt) {
  EXPECT_NE(writingOf(canvas(), A_DIALOG_HEADING), nullptr);
}

/**
 * Given a dialog screen
 * When the stack asks whether it hides the screens below
 * Then it leaves them drawn
 */
TEST_F(DialogScreenFixture, LeavesTheScreensBelowShown) {
  EXPECT_FALSE(dialog().hidesScreensBelow());
}

/**
 * Given a dialog screen and its selected button
 * When Enter is pressed and released, and the press lasts
 * Then the button runs its action
 */
TEST_F(DialogScreenFixture, AKeyThatLastsActivatesItsButton) {
  tap(sf::Keyboard::Key::Enter);

  dialog().update(PRESS_DURATION);

  EXPECT_EQ(activations(), 1);
}

/**
 * Given a dialog screen
 * When it is updated
 * Then its picture is displayed again, so a press shows on screen
 */
TEST_F(DialogScreenFixture, AnUpdateRepaintsItsMenu) {
  const int displayedOnceBuilt = canvas().displayCount;

  dialog().update(PRESS_DURATION);

  EXPECT_EQ(canvas().displayCount, displayedOnceBuilt + 1);
}

/**
 * Given a dialog screen
 * When it is drawn
 * Then its picture is laid on the target
 */
TEST_F(DialogScreenFixture, DrawingLaysItsPicture) {
  BlankRenderTarget target;

  dialog().draw(target);

  EXPECT_EQ(canvas().showCount, 1);
}
