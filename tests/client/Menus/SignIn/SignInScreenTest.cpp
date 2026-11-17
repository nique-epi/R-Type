#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include <string>
#include "DeviceLinkPhase.hpp"
#include "RecordingNavigator.hpp"
#include "SignInFixtures.hpp"
#include "SignInReturn.hpp"
#include "SignInScreenFixture.hpp"
#include "SignInTestConstants.hpp"

using rtype::client::DeviceLinkPhase;
using rtype::client::SignInReturn;
using rtype::client::testing::ASKING_SCREEN;
using rtype::client::testing::NINE_AND_A_HALF_MINUTES_LATER;
using rtype::client::testing::readingAt;
using rtype::client::testing::SHOWN_PAGE_WITH_CODE;
using rtype::client::testing::SIGN_IN_TOKEN;
using rtype::client::testing::SIGNED_IN_ACCOUNT;
using rtype::client::testing::waitingReading;
using ::testing::ElementsAre;

/**
 * Given the sign-in dialog
 * When it opens
 * Then it asks for one link
 */
TEST_F(SignInScreenFixture, AsksForALinkWhenItOpens) {
  EXPECT_EQ(deviceLinks().linksAsked(), 1U);
}

/**
 * Given the code of the link came
 * When two frames pass
 * Then the dialog opened the page of the code in the browser, once
 */
TEST_F(SignInScreenFixture, OpensThePageOfTheCodeOnce) {
  deviceLinks().publish(waitingReading(1, NINE_AND_A_HALF_MINUTES_LATER));

  frame();
  frame();

  EXPECT_THAT(openedPages(), ElementsAre(std::string{SHOWN_PAGE_WITH_CODE}));
}

/**
 * Given a link a player approved, which gave a token
 * When a frame passes
 * Then the dialog signs the account session in with that token
 */
TEST_F(SignInScreenFixture, SignsInWithTheTokenOfAnApprovedLink) {
  deviceLinks().publish(readingAt(1, DeviceLinkPhase::SignedIn));

  frame();

  EXPECT_EQ(account().askedToken(), SIGN_IN_TOKEN);
}

/**
 * Given the dialog opened from the dialog a casino screen stood in for
 * When the account of the token is read
 * Then it closes itself and that dialog, and opens the casino in its place
 */
TEST_F(SignInScreenFixture, OpensTheScreenThatAskedInPlaceOfTheDialog) {
  signInReturn() = SignInReturn{.closesAsking = true, .screen = ASKING_SCREEN};
  deviceLinks().publish(readingAt(1, DeviceLinkPhase::SignedIn));
  frame();

  account().signInAs(SIGNED_IN_ACCOUNT, std::string{SIGN_IN_TOKEN});
  frame();

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt},
                          NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt},
                          NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = ASKING_SCREEN}));
}

/**
 * Given the dialog opened from the title
 * When the account of the token is read
 * Then it only closes itself, back on the title
 */
TEST_F(SignInScreenFixture, AskedFromTheTitleItOnlyClosesItself) {
  deviceLinks().publish(readingAt(1, DeviceLinkPhase::SignedIn));
  frame();

  account().signInAs(SIGNED_IN_ACCOUNT, std::string{SIGN_IN_TOKEN});
  frame();

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}

/**
 * Given the dialog waiting for a player
 * When Escape answers BACK
 * Then the link is forgotten and the dialog closes
 */
TEST_F(SignInScreenFixture, BackForgetsTheLinkAndCloses) {
  deviceLinks().publish(waitingReading(1, NINE_AND_A_HALF_MINUTES_LATER));
  frame();

  choose(sf::Keyboard::Key::Escape);

  EXPECT_EQ(deviceLinks().cancels(), 1);
  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}

/**
 * Given a code that expired, NEW CODE offered and selected
 * When Enter activates it
 * Then the dialog asks for another link
 */
TEST_F(SignInScreenFixture, NewCodeAsksForAnotherLink) {
  deviceLinks().publish(readingAt(1, DeviceLinkPhase::Expired));
  frame();

  choose(sf::Keyboard::Key::Enter);

  EXPECT_EQ(deviceLinks().linksAsked(), 2U);
}
