#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "SignInScreenIdConstants.hpp"
#include "TitleAccountFixture.hpp"
#include "TitleAccountTestConstants.hpp"

using rtype::client::SIGN_IN_SCREEN;
using rtype::client::testing::SIGN_OUT_ACE_TEXT;
using rtype::client::testing::TITLE_ACCOUNT_TOKEN;
using ::testing::ElementsAre;

/**
 * Given the title of a guest
 * When the account bar is activated
 * Then the sign-in dialog opens over the title
 */
TEST_F(TitleAccountFixture, AGuestOpensTheSignIn) {
  chooseTheAccountBar();

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = SIGN_IN_SCREEN}));
}

/**
 * Given the title of ACE, signed in
 * When it is painted
 * Then the account bar names ACE, with the sign-out it offers
 */
TEST_F(TitleAccountFixture, TheAccountBarNamesThePlayerSignedIn) {
  signInAsAce();

  EXPECT_NE(writingOf(canvas(), SIGN_OUT_ACE_TEXT), nullptr);
}

/**
 * Given the title of ACE, signed in
 * When the account bar is activated
 * Then the session of ACE is closed on the master and the game signs out
 */
TEST_F(TitleAccountFixture, SigningOutClosesTheSessionOnTheMaster) {
  signInAsAce();

  chooseTheAccountBar();

  EXPECT_EQ(deviceLinks().signedOutToken(), TITLE_ACCOUNT_TOKEN);
  EXPECT_FALSE(account().signedIn());
}
