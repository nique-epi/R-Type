#include <gtest/gtest.h>
#include <optional>
#include <string>
#include "DeviceLinkPhase.hpp"
#include "SignInFixtures.hpp"
#include "SignInLines.hpp"
#include "SignInTestConstants.hpp"
#include "SignInText.hpp"

using rtype::client::DeviceLinkPhase;
using rtype::client::pageShownOf;
using rtype::client::SignInLines;
using rtype::client::signInLinesOf;
using rtype::client::testing::HALF_A_MINUTE_LATER;
using rtype::client::testing::LAST_MINUTE_LINE;
using rtype::client::testing::NINE_AND_A_HALF_MINUTES_LATER;
using rtype::client::testing::NINE_MINUTES_LINE;
using rtype::client::testing::PAGE_AS_SHOWN;
using rtype::client::testing::readingAt;
using rtype::client::testing::SHOWN_CODE;
using rtype::client::testing::SHOWN_PAGE;
using rtype::client::testing::SIGN_IN_NOW;
using rtype::client::testing::waitingReading;

/**
 * Given a code that expires nine and a half minutes from now
 * When the dialog writes it
 * Then it heads with the code, and gives the page and nine whole minutes
 */
TEST(SignInText, AWaitingCodeGivesItsPageAndItsWholeMinutes) {
  const SignInLines lines = signInLinesOf(
      waitingReading(1, NINE_AND_A_HALF_MINUTES_LATER), SIGN_IN_NOW, false);

  EXPECT_EQ(lines, (SignInLines{.title = std::string{SHOWN_CODE},
                                .line = std::string{NINE_MINUTES_LINE},
                                .offersNewCode = false}));
}

/**
 * Given a code that expires half a minute from now
 * When the dialog writes it
 * Then it says under a minute is left, never 0 minutes
 */
TEST(SignInText, ACodeInItsLastMinuteSaysUnderAMinute) {
  const SignInLines lines =
      signInLinesOf(waitingReading(1, HALF_A_MINUTE_LATER), SIGN_IN_NOW, false);

  EXPECT_EQ(lines.line, LAST_MINUTE_LINE);
}

/**
 * Given a link that ended refused, expired, over the quota or unreachable
 * When the dialog writes each
 * Then each offers a new code
 */
TEST(SignInText, ALinkEndedWithoutASessionOffersANewCode) {
  for (const DeviceLinkPhase phase :
       {DeviceLinkPhase::Denied, DeviceLinkPhase::Expired,
        DeviceLinkPhase::Refused, DeviceLinkPhase::Unreachable}) {
    EXPECT_TRUE(
        signInLinesOf(readingAt(1, phase), SIGN_IN_NOW, false).offersNewCode);
  }
}

/**
 * Given a link that gave a token the master then refused as a session
 * When the dialog writes it
 * Then it offers a new code
 */
TEST(SignInText, ASessionRefusedOffersANewCode) {
  EXPECT_TRUE(
      signInLinesOf(readingAt(1, DeviceLinkPhase::SignedIn), SIGN_IN_NOW, true)
          .offersNewCode);
}

/**
 * Given the page of the site, with its scheme
 * When the dialog writes it
 * Then it drops the scheme and writes it in capitals
 */
TEST(SignInText, WritesThePageWithoutItsSchemeInCapitals) {
  EXPECT_EQ(pageShownOf(std::string{SHOWN_PAGE}), PAGE_AS_SHOWN);
}
