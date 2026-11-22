#include <gtest/gtest.h>
#include <optional>
#include <string>
#include "DeviceLinkPhase.hpp"
#include "Language.hpp"
#include "SignInFixtures.hpp"
#include "SignInLines.hpp"
#include "SignInTestConstants.hpp"
#include "SignInText.hpp"

using rtype::client::DeviceLinkPhase;
using rtype::client::Language;
using rtype::client::pageShownOf;
using rtype::client::SignInLines;
using rtype::client::signInLinesOf;
using rtype::client::SignInStanding;
using rtype::client::testing::FRENCH_NINE_MINUTES_VALIDITY;
using rtype::client::testing::HALF_A_MINUTE_LATER;
using rtype::client::testing::LAST_MINUTE_VALIDITY;
using rtype::client::testing::NINE_AND_A_HALF_MINUTES_LATER;
using rtype::client::testing::NINE_MINUTES_VALIDITY;
using rtype::client::testing::PAGE_AS_SHOWN;
using rtype::client::testing::PAGE_LINE;
using rtype::client::testing::readingAt;
using rtype::client::testing::SHOWN_CODE;
using rtype::client::testing::SHOWN_PAGE;
using rtype::client::testing::SIGN_IN_NOW;
using rtype::client::testing::waitingReading;

/**
 * Given a code that expires nine and a half minutes from now
 * When the dialog writes it
 * Then it heads with the code, gives the page, and says nine whole minutes
 * are left on a line of their own
 */
TEST(SignInText, AWaitingCodeGivesItsPageAndItsWholeMinutes) {
  const SignInLines lines = signInLinesOf(
      waitingReading(1, NINE_AND_A_HALF_MINUTES_LATER),
      SignInStanding{.now = SIGN_IN_NOW, .sessionRefused = false});

  EXPECT_EQ(lines, (SignInLines{.title = std::string{SHOWN_CODE},
                                .line = std::string{PAGE_LINE},
                                .validity = std::string{NINE_MINUTES_VALIDITY},
                                .offersNewCode = false}));
}

/**
 * Given the client in French and a code that expires nine and a half
 * minutes from now
 * When the dialog writes it
 * Then it says in French that nine whole minutes are left
 */
TEST(SignInText, AWaitingCodeTellsItsMinutesInFrench) {
  const SignInLines lines =
      signInLinesOf(waitingReading(1, NINE_AND_A_HALF_MINUTES_LATER),
                    SignInStanding{.now = SIGN_IN_NOW,
                                   .sessionRefused = false,
                                   .language = Language::French});

  EXPECT_EQ(lines.validity.value_or(std::string{}),
            FRENCH_NINE_MINUTES_VALIDITY);
}

/**
 * Given a code that expires half a minute from now
 * When the dialog writes it
 * Then it says under a minute is left, never 0 minutes
 */
TEST(SignInText, ACodeInItsLastMinuteSaysUnderAMinute) {
  const SignInLines lines = signInLinesOf(
      waitingReading(1, HALF_A_MINUTE_LATER),
      SignInStanding{.now = SIGN_IN_NOW, .sessionRefused = false});

  EXPECT_EQ(lines.validity.value_or(std::string{}), LAST_MINUTE_VALIDITY);
}

/**
 * Given a code that expired
 * When the dialog writes it
 * Then it says nothing of a time left
 */
TEST(SignInText, AnExpiredCodeHasNoTimeLeft) {
  EXPECT_FALSE(
      signInLinesOf(readingAt(1, DeviceLinkPhase::Expired),
                    SignInStanding{.now = SIGN_IN_NOW, .sessionRefused = false})
          .validity.has_value());
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
    EXPECT_TRUE(signInLinesOf(
                    readingAt(1, phase),
                    SignInStanding{.now = SIGN_IN_NOW, .sessionRefused = false})
                    .offersNewCode);
  }
}

/**
 * Given a link that gave a token the master then refused as a session
 * When the dialog writes it
 * Then it offers a new code
 */
TEST(SignInText, ASessionRefusedOffersANewCode) {
  EXPECT_TRUE(
      signInLinesOf(readingAt(1, DeviceLinkPhase::SignedIn),
                    SignInStanding{.now = SIGN_IN_NOW, .sessionRefused = true})
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
