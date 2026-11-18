#include <gtest/gtest.h>
#include <cstddef>
#include <string>
#include <variant>
#include "AccountsTestConstants.hpp"
#include "CreatedDeviceLink.hpp"
#include "CreatedSession.hpp"
#include "DeviceLinkDecision.hpp"
#include "DeviceLinkRefusal.hpp"
#include "DeviceLinkState.hpp"
#include "DeviceLinkStatus.hpp"
#include "DeviceLinksServiceFixture.hpp"
#include "DeviceLinksServiceTestConstants.hpp"
#include "MasterMessageText.hpp"
#include "SessionOrigin.hpp"

using rtype::master::DeviceLinkRefusal;
using rtype::master::SessionOrigin;
using rtype::network::CreatedDeviceLink;
using rtype::network::CreatedSession;
using rtype::network::DeviceLinkDecision;
using rtype::network::DeviceLinkState;
using rtype::network::DeviceLinkStatus;

/**
 * Given a game that asks for a link
 * When the master opens it
 * Then it holds a secret of a link, a short code of the alphabet, the page of
 * the site with and without the code, and it expires ten minutes later
 */
TEST_F(DeviceLinksServiceFixture, OpensALinkWithASecretAndAShortCode) {
  const CreatedDeviceLink link = openLink();

  EXPECT_TRUE(rtype::network::isWellFormedDeviceLinkSecret(link.secret));
  EXPECT_EQ(rtype::network::deviceLinkUserCodeOf(link.userCode)
                .value_or(std::string{}),
            link.userCode);
  EXPECT_EQ(link.verificationUri, TEST_LINK_PAGE);
  EXPECT_EQ(link.verificationUriComplete,
            std::string{CODE_QUERY_START} + link.userCode);
  EXPECT_EQ(link.expiresAt, TEN_MINUTES_LATER);
}

/**
 * Given a link no one decided on
 * When its game reads it once its ten minutes passed
 * Then it reads expired
 */
TEST_F(DeviceLinksServiceFixture, ALinkLeftUndecidedExpires) {
  const CreatedDeviceLink link = openLink();

  const auto read = links().readOwn(link.secret, TEN_MINUTES_LATER);

  ASSERT_TRUE(std::holds_alternative<DeviceLinkState>(read));
  EXPECT_EQ(std::get<DeviceLinkState>(read).status, DeviceLinkStatus::Expired);
}

/**
 * Given a link ACE refused
 * When its game reads it once its ten minutes passed
 * Then it still reads denied, so the game says it was refused
 */
TEST_F(DeviceLinksServiceFixture, ARefusedLinkStaysDeniedPastItsExpiry) {
  const CreatedDeviceLink link = openLink();
  ASSERT_TRUE(std::holds_alternative<DeviceLinkState>(
      decideAsAce(link, DeviceLinkDecision::Denied, LINK_OPENED_AT)));

  const auto read = links().readOwn(link.secret, TEN_MINUTES_LATER);

  ASSERT_TRUE(std::holds_alternative<DeviceLinkState>(read));
  EXPECT_EQ(std::get<DeviceLinkState>(read).status, DeviceLinkStatus::Denied);
}

/**
 * Given a link ACE approved
 * When its game trades it for a session once its ten minutes passed
 * Then the master refuses it as expired
 */
TEST_F(DeviceLinksServiceFixture, AnApprovedLinkOpensNoSessionPastItsExpiry) {
  const CreatedDeviceLink link = openLink();
  ASSERT_TRUE(std::holds_alternative<DeviceLinkState>(
      decideAsAce(link, DeviceLinkDecision::Approved, LINK_OPENED_AT)));

  const auto traded = links().openSession(
      link.secret, SessionOrigin{.address = std::string{GAME_ADDRESS},
                                 .openedAt = TEN_MINUTES_LATER});

  ASSERT_TRUE(std::holds_alternative<DeviceLinkRefusal>(traded));
  EXPECT_EQ(std::get<DeviceLinkRefusal>(traded), DeviceLinkRefusal::Expired);
}

/**
 * Given a link no one decided on
 * When ACE approves it once its ten minutes passed
 * Then the master refuses the decision
 */
TEST_F(DeviceLinksServiceFixture, AnExpiredLinkTakesNoDecision) {
  const CreatedDeviceLink link = openLink();

  const auto decided =
      decideAsAce(link, DeviceLinkDecision::Approved, TEN_MINUTES_LATER);

  ASSERT_TRUE(std::holds_alternative<DeviceLinkRefusal>(decided));
  EXPECT_EQ(std::get<DeviceLinkRefusal>(decided),
            DeviceLinkRefusal::AlreadyDecided);
}

/**
 * Given a game that read its link when it opened
 * When it reads it again three seconds later
 * Then the master slows it down
 */
TEST_F(DeviceLinksServiceFixture, AReadBeforeTheIntervalIsSlowedDown) {
  const CreatedDeviceLink link = openLink();

  const auto read = links().readOwn(link.secret, THREE_SECONDS_LATER);

  ASSERT_TRUE(std::holds_alternative<DeviceLinkRefusal>(read));
  EXPECT_EQ(std::get<DeviceLinkRefusal>(read), DeviceLinkRefusal::TooSoon);
}

/**
 * Given a game that read its link when it opened
 * When it reads it again five seconds later, the interval it was given
 * Then it reads the link
 */
TEST_F(DeviceLinksServiceFixture, AReadAfterTheIntervalIsAnswered) {
  const CreatedDeviceLink link = openLink();

  const auto read = links().readOwn(link.secret, FIVE_SECONDS_LATER);

  ASSERT_TRUE(std::holds_alternative<DeviceLinkState>(read));
  EXPECT_EQ(std::get<DeviceLinkState>(read).status, DeviceLinkStatus::Pending);
}

/**
 * Given an address that asked for its ten links of the hour
 * When it asks for another in the same second
 * Then the master refuses it over the quota
 */
TEST_F(DeviceLinksServiceFixture, AnAddressPastItsQuotaGetsNoLink) {
  for (std::size_t asked = 0; asked < ALLOWED_LINKS_PER_HOUR; ++asked) {
    ASSERT_FALSE(openLink().secret.empty());
  }

  const auto refused =
      links().create(std::string{GAME_ADDRESS}, LINK_OPENED_AT);

  ASSERT_TRUE(std::holds_alternative<DeviceLinkRefusal>(refused));
  EXPECT_EQ(std::get<DeviceLinkRefusal>(refused),
            DeviceLinkRefusal::TooManyCreations);
}

/**
 * Given an address that asked for its ten links of the hour
 * When it asks for another an hour and a second later
 * Then the master opens it: the quota counts the last hour only
 */
TEST_F(DeviceLinksServiceFixture, TheQuotaForgetsTheLinksOfAnHourAgo) {
  for (std::size_t asked = 0; asked < ALLOWED_LINKS_PER_HOUR; ++asked) {
    ASSERT_FALSE(openLink().secret.empty());
  }

  const auto opened =
      links().create(std::string{GAME_ADDRESS}, AN_HOUR_AND_A_SECOND_LATER);

  EXPECT_TRUE(std::holds_alternative<CreatedDeviceLink>(opened));
}

/**
 * Given a link ACE approved
 * When its game trades it for a session within its ten minutes
 * Then the master opens a session of ACE
 */
TEST_F(DeviceLinksServiceFixture, AnApprovedLinkOpensASessionOfItsApprover) {
  const CreatedDeviceLink link = openLink();
  ASSERT_TRUE(std::holds_alternative<DeviceLinkState>(
      decideAsAce(link, DeviceLinkDecision::Approved, LINK_OPENED_AT)));

  const auto traded = links().openSession(
      link.secret, SessionOrigin{.address = std::string{GAME_ADDRESS},
                                 .openedAt = FIVE_SECONDS_LATER});

  ASSERT_TRUE(std::holds_alternative<CreatedSession>(traded));
  EXPECT_EQ(std::get<CreatedSession>(traded).user.nickname, ACE_NICKNAME);
}
