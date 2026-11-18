#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <string_view>
#include "CreatedDeviceLink.hpp"
#include "DeviceLinkDecision.hpp"
#include "DeviceLinkState.hpp"
#include "DeviceLinkStatus.hpp"
#include "DeviceLinkTextTestConstants.hpp"
#include "MasterMessageText.hpp"
#include "NetworkException.hpp"

using rtype::network::CreatedDeviceLink;
using rtype::network::createdDeviceLinkFromText;
using rtype::network::createdDeviceLinkText;
using rtype::network::decisionMessageFromText;
using rtype::network::decisionMessageText;
using rtype::network::DeviceLinkDecision;
using rtype::network::DeviceLinkState;
using rtype::network::deviceLinkStateFromText;
using rtype::network::deviceLinkStateText;
using rtype::network::DeviceLinkStatus;
using rtype::network::deviceLinkUserCodeOf;
using rtype::network::InvalidMasterMessageException;
using rtype::network::isWellFormedDeviceLinkSecret;
using ::testing::HasSubstr;

/**
 * Given a link the master opened for a game
 * When it is written as the master answers it, then read back
 * Then it reads as the same secret, code, pages, expiry and interval
 */
TEST(DeviceLinkText, ReadsBackAWrittenLink) {
  const CreatedDeviceLink link{
      .secret = std::string{WELL_FORMED_SECRET},
      .userCode = std::string{KEPT_USER_CODE},
      .verificationUri = std::string{LINK_PAGE},
      .verificationUriComplete = std::string{LINK_PAGE_WITH_CODE},
      .expiresAt = LINK_EXPIRES_AT,
      .interval = LINK_INTERVAL};

  EXPECT_EQ(createdDeviceLinkFromText(createdDeviceLinkText(link)), link);
}

/**
 * Given an approved link as the master shows it
 * When it is written, then read back
 * Then it reads as the same code, status, dates and interval
 */
TEST(DeviceLinkText, ReadsBackAWrittenState) {
  const DeviceLinkState state{.userCode = std::string{KEPT_USER_CODE},
                              .status = DeviceLinkStatus::Approved,
                              .createdAt = LINK_CREATED_AT,
                              .expiresAt = LINK_EXPIRES_AT,
                              .interval = LINK_INTERVAL};

  EXPECT_EQ(deviceLinkStateFromText(deviceLinkStateText(state)), state);
}

/**
 * Given a state whose interval is 0 seconds
 * When the game reads it
 * Then it is refused at /interval, so the game never reads in a tight loop
 */
TEST(DeviceLinkText, RefusesAnIntervalOfNoSecond) {
  try {
    static_cast<void>(deviceLinkStateFromText(NO_INTERVAL_STATE));
    FAIL() << "a state with no interval was read";
  } catch (const InvalidMasterMessageException& refused) {
    EXPECT_THAT(refused.what(), HasSubstr(INTERVAL_POINTER));
  }
}

/**
 * Given a denial a player sends
 * When it is written as the body of the decision, then read back
 * Then it reads as a denial
 */
TEST(DeviceLinkText, ReadsBackADecision) {
  EXPECT_EQ(
      decisionMessageFromText(decisionMessageText(DeviceLinkDecision::Denied)),
      DeviceLinkDecision::Denied);
}

/**
 * Given a body whose decision is "maybe"
 * When the master reads it
 * Then it is refused at /decision
 */
TEST(DeviceLinkText, RefusesAWordThatIsNoDecision) {
  try {
    static_cast<void>(decisionMessageFromText(MAYBE_DECISION));
    FAIL() << "maybe was read as a decision";
  } catch (const InvalidMasterMessageException& refused) {
    EXPECT_THAT(refused.what(), HasSubstr(DECISION_POINTER));
  }
}

/**
 * Given a secret of the shape the master draws, and one with the prefix of a
 * session token
 * When each is checked
 * Then only the first is the secret of a link
 */
TEST(DeviceLinkText, TellsTheSecretOfALinkFromAToken) {
  EXPECT_TRUE(isWellFormedDeviceLinkSecret(WELL_FORMED_SECRET));
  EXPECT_FALSE(isWellFormedDeviceLinkSecret(SECRET_WITH_A_TOKEN_PREFIX));
}

/**
 * Given a short code typed in lowercase, without its dash, or as kept
 * When each is read
 * Then each reads as the code the master keeps, in capitals with its dash
 */
TEST(DeviceLinkText, ReadsAShortCodeHoweverItIsTyped) {
  for (const std::string_view typed :
       {LOWERCASE_USER_CODE, USER_CODE_WITHOUT_DASH, KEPT_USER_CODE}) {
    SCOPED_TRACE(typed);
    EXPECT_EQ(deviceLinkUserCodeOf(typed).value_or(std::string{}),
              KEPT_USER_CODE);
  }
}

/**
 * Given a code with a vowel, a digit, a letter short, a letter too many, or
 * nothing
 * When each is read
 * Then none is a short code
 */
TEST(DeviceLinkText, RefusesATextThatIsNoShortCode) {
  for (const std::string_view typed :
       {USER_CODE_WITH_A_VOWEL, USER_CODE_WITH_A_DIGIT, SHORT_USER_CODE,
        LONG_USER_CODE, EMPTY_USER_CODE}) {
    SCOPED_TRACE(typed);
    EXPECT_FALSE(deviceLinkUserCodeOf(typed).has_value());
  }
}
