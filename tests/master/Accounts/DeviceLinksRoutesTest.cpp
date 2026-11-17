#include <gtest/gtest.h>
#include <httplib.h>
#include <string>
#include "AccountsRoutesFixtures.hpp"
#include "AccountsRoutesTestConstants.hpp"
#include "AccountsTestConstants.hpp"
#include "DeviceLinksRoutesFixture.hpp"
#include "DeviceLinksRoutesFixtures.hpp"
#include "DeviceLinksRoutesTestConstants.hpp"
#include "MasterTestConstants.hpp"
#include "PlayerLinkCall.hpp"
#include "RouteCall.hpp"
#include "ServersRoutesTestConstants.hpp"

/**
 * Given a master
 * When a game asks for a device link over HTTPS
 * Then the master answers 201 with a secret of a link, and names the link of
 * that secret in Location
 */
TEST_F(DeviceLinksRoutesFixture, CreatesALinkOverHttps) {
  const httplib::Result created = createLink();

  ASSERT_TRUE(created);
  EXPECT_EQ(created->status, EXPECTED_CREATED);
  EXPECT_TRUE(
      memberIn(created, SECRET_MEMBER_NAME).starts_with(LINK_SECRET_START));
  EXPECT_EQ(created->get_header_value(std::string{LOCATION_HEADER_NAME}),
            OWN_DEVICE_LINK_URL);
}

/**
 * Given a master
 * When a game asks for a device link in plain HTTP, no proxy saying otherwise
 * Then the master refuses it with 403, so a secret never crosses in clear
 */
TEST_F(DeviceLinksRoutesFixture, RefusesALinkInPlainHttp) {
  const httplib::Result refused = send(RouteCall{.method = POST_METHOD,
                                                 .path = DEVICE_LINKS_URL,
                                                 .key = {},
                                                 .body = {},
                                                 .overHttps = false});

  ASSERT_TRUE(refused);
  EXPECT_EQ(refused->status, EXPECTED_FORBIDDEN);
}

/**
 * Given a link a game just asked for
 * When its game reads it right away, before the interval of 5 seconds
 * Then the master slows it down with 429 and Retry-After: 5
 */
TEST_F(DeviceLinksRoutesFixture, AReadBeforeTheIntervalIsSlowedDown) {
  const std::string secret = memberIn(createLink(), SECRET_MEMBER_NAME);

  const httplib::Result slowed = readOwnLink(secret);

  ASSERT_TRUE(slowed);
  EXPECT_EQ(slowed->status, EXPECTED_TOO_MANY_REQUESTS);
  EXPECT_EQ(slowed->get_header_value(std::string{RETRY_AFTER_HEADER_NAME}),
            FIVE_SECONDS_TEXT);
}

/**
 * Given a link a game just asked for, and ACE signed in
 * When ACE reads it by its code
 * Then the master answers 200: the link waits for a decision
 */
TEST_F(DeviceLinksRoutesFixture, APlayerReadsAPendingLinkByItsCode) {
  const std::string code = memberIn(createLink(), USER_CODE_MEMBER_NAME);

  const httplib::Result read = readLink(
      PlayerLinkCall{.userCode = code, .token = aceToken(), .body = {}});

  ASSERT_TRUE(read);
  EXPECT_EQ(read->status, EXPECTED_OK);
  EXPECT_EQ(memberIn(read, STATUS_MEMBER_NAME), PENDING_STATUS_NAME);
}

/**
 * Given a link a game just asked for, and ACE signed in
 * When ACE types its code in lowercase, without its dash
 * Then the master finds the link and names its code as it keeps it
 */
TEST_F(DeviceLinksRoutesFixture, APlayerTypesTheCodeInAnyCase) {
  const std::string code = memberIn(createLink(), USER_CODE_MEMBER_NAME);

  const httplib::Result read =
      readLink(PlayerLinkCall{.userCode = lowercaseWithoutSeparator(code),
                              .token = aceToken(),
                              .body = {}});

  ASSERT_TRUE(read);
  EXPECT_EQ(memberIn(read, USER_CODE_MEMBER_NAME), code);
}

/**
 * Given ACE signed in
 * When ACE reads a code no link holds
 * Then the master answers 404
 */
TEST_F(DeviceLinksRoutesFixture, AnUnknownCodeIsNotFound) {
  const httplib::Result read = readLink(PlayerLinkCall{
      .userCode = UNKNOWN_USER_CODE, .token = aceToken(), .body = {}});

  ASSERT_TRUE(read);
  EXPECT_EQ(read->status, EXPECTED_NOT_FOUND);
}

/**
 * Given a link a game just asked for
 * When a request with no session approves it
 * Then the master refuses it with 401: only an account decides
 */
TEST_F(DeviceLinksRoutesFixture, ADecisionNeedsASession) {
  const std::string code = memberIn(createLink(), USER_CODE_MEMBER_NAME);

  const httplib::Result refused = decide(
      PlayerLinkCall{.userCode = code, .token = {}, .body = APPROVAL_BODY});

  ASSERT_TRUE(refused);
  EXPECT_EQ(refused->status, EXPECTED_UNAUTHORIZED);
}

/**
 * Given a link a game just asked for, and ACE signed in
 * When ACE sends a decision that is neither approved nor denied
 * Then the master refuses it with 400
 */
TEST_F(DeviceLinksRoutesFixture, AWordThatIsNoDecisionIsRefused) {
  const std::string code = memberIn(createLink(), USER_CODE_MEMBER_NAME);

  const httplib::Result refused = decide(PlayerLinkCall{
      .userCode = code, .token = aceToken(), .body = NO_DECISION_BODY});

  ASSERT_TRUE(refused);
  EXPECT_EQ(refused->status, EXPECTED_BAD_REQUEST);
}

/**
 * Given a link ACE approved
 * When its game trades it for a session, then reads the account of that
 * session
 * Then the master answers 201, and the session is one of ACE
 */
TEST_F(DeviceLinksRoutesFixture, AnApprovedLinkOpensASessionOfItsApprover) {
  const httplib::Result link = createLink();
  ASSERT_TRUE(
      decide(PlayerLinkCall{.userCode = memberIn(link, USER_CODE_MEMBER_NAME),
                            .token = aceToken(),
                            .body = APPROVAL_BODY}));

  const httplib::Result traded = tradeLink(memberIn(link, SECRET_MEMBER_NAME));

  ASSERT_TRUE(traded);
  EXPECT_EQ(traded->status, EXPECTED_CREATED);
  EXPECT_EQ(memberIn(send(readOwnUserCall(memberIn(traded, TOKEN_MEMBER_NAME))),
                     USER_NICKNAME_MEMBER_NAME),
            ACE_NICKNAME);
}

/**
 * Given a link ACE approved, and traded once for a session
 * When its game trades it again
 * Then the master refuses it with 401: a link opens one session
 */
TEST_F(DeviceLinksRoutesFixture, ALinkOpensOneSession) {
  const httplib::Result link = createLink();
  ASSERT_TRUE(
      decide(PlayerLinkCall{.userCode = memberIn(link, USER_CODE_MEMBER_NAME),
                            .token = aceToken(),
                            .body = APPROVAL_BODY}));
  const std::string secret = memberIn(link, SECRET_MEMBER_NAME);
  ASSERT_TRUE(tradeLink(secret));

  const httplib::Result again = tradeLink(secret);

  ASSERT_TRUE(again);
  EXPECT_EQ(again->status, EXPECTED_UNAUTHORIZED);
}

/**
 * Given a link no one decided on
 * When its game trades it for a session
 * Then the master refuses it with 409: it still waits
 */
TEST_F(DeviceLinksRoutesFixture, APendingLinkOpensNoSessionYet) {
  const httplib::Result traded =
      tradeLink(memberIn(createLink(), SECRET_MEMBER_NAME));

  ASSERT_TRUE(traded);
  EXPECT_EQ(traded->status, EXPECTED_CONFLICT);
}

/**
 * Given a link ACE refused
 * When its game trades it for a session
 * Then the master refuses it with 403
 */
TEST_F(DeviceLinksRoutesFixture, ARefusedLinkOpensNoSession) {
  const httplib::Result link = createLink();
  ASSERT_TRUE(
      decide(PlayerLinkCall{.userCode = memberIn(link, USER_CODE_MEMBER_NAME),
                            .token = aceToken(),
                            .body = DENIAL_BODY}));

  const httplib::Result traded = tradeLink(memberIn(link, SECRET_MEMBER_NAME));

  ASSERT_TRUE(traded);
  EXPECT_EQ(traded->status, EXPECTED_FORBIDDEN);
}

/**
 * Given a link ACE approved
 * When ACE sends the same approval again
 * Then the master answers 200 again: a PUT sent twice gives the same result
 */
TEST_F(DeviceLinksRoutesFixture, TheSameDecisionSentTwiceIsAnsweredTheSame) {
  const std::string code = memberIn(createLink(), USER_CODE_MEMBER_NAME);
  const std::string token = aceToken();
  ASSERT_TRUE(decide(
      PlayerLinkCall{.userCode = code, .token = token, .body = APPROVAL_BODY}));

  const httplib::Result again = decide(
      PlayerLinkCall{.userCode = code, .token = token, .body = APPROVAL_BODY});

  ASSERT_TRUE(again);
  EXPECT_EQ(again->status, EXPECTED_OK);
}

/**
 * Given a link ACE approved
 * When ACE then denies it
 * Then the master refuses it with 409: a decision is written once
 */
TEST_F(DeviceLinksRoutesFixture, AContraryDecisionIsRefused) {
  const std::string code = memberIn(createLink(), USER_CODE_MEMBER_NAME);
  const std::string token = aceToken();
  ASSERT_TRUE(decide(
      PlayerLinkCall{.userCode = code, .token = token, .body = APPROVAL_BODY}));

  const httplib::Result contrary = decide(
      PlayerLinkCall{.userCode = code, .token = token, .body = DENIAL_BODY});

  ASSERT_TRUE(contrary);
  EXPECT_EQ(contrary->status, EXPECTED_CONFLICT);
}

/**
 * Given a link its game forgot, as BACK does
 * When ACE reads it by its code
 * Then the master answers 404: the link is gone
 */
TEST_F(DeviceLinksRoutesFixture, AForgottenLinkIsGone) {
  const httplib::Result link = createLink();
  const httplib::Result forgotten =
      forgetLink(memberIn(link, SECRET_MEMBER_NAME));
  ASSERT_TRUE(forgotten);
  ASSERT_EQ(forgotten->status, EXPECTED_NO_CONTENT);

  const httplib::Result read =
      readLink(PlayerLinkCall{.userCode = memberIn(link, USER_CODE_MEMBER_NAME),
                              .token = aceToken(),
                              .body = {}});

  ASSERT_TRUE(read);
  EXPECT_EQ(read->status, EXPECTED_NOT_FOUND);
}

/**
 * Given a link ACE approved and its game traded for a session
 * When the journal of the master is read
 * Then no line holds a secret of a link or a token of a session
 */
TEST_F(DeviceLinksRoutesFixture, TheJournalHoldsNoSecretAndNoToken) {
  const httplib::Result link = createLink();
  ASSERT_TRUE(
      decide(PlayerLinkCall{.userCode = memberIn(link, USER_CODE_MEMBER_NAME),
                            .token = aceToken(),
                            .body = APPROVAL_BODY}));
  ASSERT_TRUE(tradeLink(memberIn(link, SECRET_MEMBER_NAME)));

  EXPECT_TRUE(master().journal().linesHolding(LINK_SECRET_START).empty());
  EXPECT_TRUE(master().journal().linesHolding(SESSION_TOKEN_START).empty());
}
