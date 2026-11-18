#include <gtest/gtest.h>
#include <cstdint>
#include <optional>
#include <string>
#include "CannedAnswer.hpp"
#include "DeviceLinkPhase.hpp"
#include "DeviceLinkReading.hpp"
#include "DeviceLinkStatus.hpp"
#include "MasterDeviceLinkFlowFixture.hpp"
#include "MasterDeviceLinkFlowTestConstants.hpp"
#include "RecordedRequest.hpp"

using rtype::client::DeviceLinkPhase;
using rtype::client::DeviceLinkReading;
using rtype::client::testing::CREATE_LINK_ROUTE;
using rtype::client::testing::FLOW_BEARER;
using rtype::client::testing::FLOW_PAGE_WITH_CODE;
using rtype::client::testing::FLOW_TOKEN;
using rtype::client::testing::FLOW_TOO_MANY_REQUESTS_STATUS;
using rtype::client::testing::FLOW_USER_CODE;
using rtype::client::testing::MasterDeviceLinkFlowFixture;
using rtype::client::testing::OWN_LINK_PATH;
using rtype::client::testing::SECRET_START;
using rtype::network::DeviceLinkStatus;

/**
 * Given a master that opens a link
 * When the flow asks for one
 * Then it publishes the code and the page to validate it on, waiting
 */
TEST_F(MasterDeviceLinkFlowFixture, ShowsTheCodeOfANewLink) {
  const std::uint64_t ask = flow().askLink();

  const std::optional<DeviceLinkReading> reading =
      readingSoon(ask, DeviceLinkPhase::Waiting);

  ASSERT_TRUE(reading.has_value());
  EXPECT_EQ(reading.value_or(DeviceLinkReading{}).userCode, FLOW_USER_CODE);
  EXPECT_EQ(reading.value_or(DeviceLinkReading{}).verificationUriComplete,
            FLOW_PAGE_WITH_CODE);
}

/**
 * Given a link a player approved on the site
 * When the flow reads it
 * Then it trades it and publishes the token of the session
 */
TEST_F(MasterDeviceLinkFlowFixture, AnApprovedLinkGivesTheTokenOfItsSession) {
  showLinkAs(DeviceLinkStatus::Approved);

  const std::optional<DeviceLinkReading> reading =
      readingSoon(flow().askLink(), DeviceLinkPhase::SignedIn);

  ASSERT_TRUE(reading.has_value());
  EXPECT_EQ(reading.value_or(DeviceLinkReading{}).token, FLOW_TOKEN);
}

/**
 * Given a link a player refused on the site
 * When the flow reads it
 * Then it publishes the link denied, with no token
 */
TEST_F(MasterDeviceLinkFlowFixture, ARefusedLinkEndsDenied) {
  showLinkAs(DeviceLinkStatus::Denied);

  const std::optional<DeviceLinkReading> reading =
      readingSoon(flow().askLink(), DeviceLinkPhase::Denied);

  ASSERT_TRUE(reading.has_value());
  EXPECT_TRUE(reading.value_or(DeviceLinkReading{}).token.empty());
}

/**
 * Given a master past its quota of links for this address
 * When the flow asks for a link
 * Then it publishes the refusal
 */
TEST_F(MasterDeviceLinkFlowFixture, AQuotaRefusalEndsRefused) {
  master().answer(
      std::string{CREATE_LINK_ROUTE},
      CannedAnswer{.status = FLOW_TOO_MANY_REQUESTS_STATUS, .body = {}});

  EXPECT_TRUE(readingSoon(flow().askLink(), DeviceLinkPhase::Refused));
}

/**
 * Given a link the flow shows
 * When the player cancels it
 * Then the flow asks the master to forget it, its secret as Bearer
 */
TEST_F(MasterDeviceLinkFlowFixture, ACancelledLinkIsForgottenOnTheMaster) {
  ASSERT_TRUE(readingSoon(flow().askLink(), DeviceLinkPhase::Waiting));

  flow().askCancel();

  EXPECT_TRUE(forgottenSoon());
}

/**
 * Given a link waiting for a player
 * When the flow reads it
 * Then it names the link by its secret as Bearer, never in the path
 */
TEST_F(MasterDeviceLinkFlowFixture, TheSecretTravelsAsBearerOnly) {
  showLinkAs(DeviceLinkStatus::Denied);
  ASSERT_TRUE(readingSoon(flow().askLink(), DeviceLinkPhase::Denied));

  for (const RecordedRequest& request : master().requests()) {
    if (request.path == OWN_LINK_PATH) {
      EXPECT_EQ(request.authorization, FLOW_BEARER);
    }
    EXPECT_EQ(request.path.find(SECRET_START), std::string::npos);
  }
}
