#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include "MenuScreenIdConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "SignInScreenIdConstants.hpp"
#include "TitleAccountFixture.hpp"
#include "TitleAccountTestConstants.hpp"

using rtype::client::OPTIONS_SCREEN;
using rtype::client::SIGN_IN_SCREEN;
using rtype::client::testing::BALANCE_CAPTION_TEXT;
using rtype::client::testing::SIGN_IN_CORNER_PIXEL;
using rtype::client::testing::SIGN_IN_TEXT;
using rtype::client::testing::SIGN_OUT_TEXT;
using rtype::client::testing::TITLE_ACCOUNT_NICKNAME;
using rtype::client::testing::TITLE_ACCOUNT_TOKEN;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

/**
 * Given the title of a guest
 * When I is pressed and released
 * Then the sign-in dialog opens over the title
 */
TEST_F(TitleAccountFixture, AGuestOpensTheSignInWithItsKey) {
  press(sf::Keyboard::Key::I);
  release(sf::Keyboard::Key::I);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = SIGN_IN_SCREEN}));
}

/**
 * Given the title of a guest
 * When the top-right corner is clicked
 * Then the sign-in dialog opens over the title
 */
TEST_F(TitleAccountFixture, AGuestOpensTheSignInWithAClickOnTheCorner) {
  clickAt(SIGN_IN_CORNER_PIXEL);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = SIGN_IN_SCREEN}));
}

/**
 * Given the title of a guest
 * When it is painted
 * Then it offers SIGN IN, and neither a balance nor SIGN OUT
 */
TEST_F(TitleAccountFixture, AGuestSeesSignInInPlaceOfTheBalance) {
  EXPECT_NE(writingOf(canvas(), SIGN_IN_TEXT), nullptr);
  EXPECT_EQ(writingOf(canvas(), BALANCE_CAPTION_TEXT), nullptr);
  EXPECT_EQ(writingOf(canvas(), SIGN_OUT_TEXT), nullptr);
}

/**
 * Given the title of ACE, signed in
 * When it is painted
 * Then it shows the balance, ACE and SIGN OUT, and no SIGN IN
 */
TEST_F(TitleAccountFixture, APlayerSignedInSeesTheBalanceAndItsNickname) {
  signInAsAce();

  EXPECT_NE(writingOf(canvas(), BALANCE_CAPTION_TEXT), nullptr);
  EXPECT_NE(writingOf(canvas(), TITLE_ACCOUNT_NICKNAME), nullptr);
  EXPECT_NE(writingOf(canvas(), SIGN_OUT_TEXT), nullptr);
  EXPECT_EQ(writingOf(canvas(), SIGN_IN_TEXT), nullptr);
}

/**
 * Given the title of ACE, signed in
 * When O is pressed and released
 * Then the session of ACE is closed on the master and the game signs out
 */
TEST_F(TitleAccountFixture, SigningOutClosesTheSessionOnTheMaster) {
  signInAsAce();

  press(sf::Keyboard::Key::O);
  release(sf::Keyboard::Key::O);

  EXPECT_EQ(deviceLinks().signedOutToken(), TITLE_ACCOUNT_TOKEN);
  EXPECT_FALSE(account().signedIn());
}

/**
 * Given the title of a guest
 * When O is pressed and released
 * Then no chip answers it: no click plays and no screen opens
 */
TEST_F(TitleAccountFixture, TheSignOutKeyDoesNothingForAGuest) {
  press(sf::Keyboard::Key::O);
  release(sf::Keyboard::Key::O);

  EXPECT_THAT(sounds().played(), IsEmpty());
  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given the title of a guest
 * When I is held down, its press repeated before its release
 * Then the sign-in dialog opens once
 */
TEST_F(TitleAccountFixture, AHeldSignInKeyOpensTheDialogOnce) {
  press(sf::Keyboard::Key::I);
  press(sf::Keyboard::Key::I);
  release(sf::Keyboard::Key::I);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = SIGN_IN_SCREEN}));
}

/**
 * Given the title of ACE, signed in, PLAY selected
 * When Up is pressed twice, then Enter
 * Then SETTINGS opens: going up passes QUIT, then SETTINGS, with no account
 * bar left in the column
 */
TEST_F(TitleAccountFixture, TheColumnNoLongerHoldsTheAccount) {
  signInAsAce();

  press(sf::Keyboard::Key::Up);
  release(sf::Keyboard::Key::Up);
  press(sf::Keyboard::Key::Up);
  release(sf::Keyboard::Key::Up);
  press(sf::Keyboard::Key::Enter);
  release(sf::Keyboard::Key::Enter);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = OPTIONS_SCREEN}));
}
