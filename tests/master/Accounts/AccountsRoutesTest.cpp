#include <gtest/gtest.h>
#include <httplib.h>
#include <cstddef>
#include <string>
#include <vector>
#include "AccountsRoutesFixture.hpp"
#include "AccountsRoutesFixtures.hpp"
#include "AccountsRoutesTestConstants.hpp"
#include "AccountsTestConstants.hpp"
#include "MasterException.hpp"
#include "MasterTestConstants.hpp"
#include "RouteCall.hpp"
#include "RouteCallFixtures.hpp"
#include "RunningMaster.hpp"
#include "UserCredentials.hpp"

/**
 * Given a master with its key of the passwords
 * When ACE creates an account over HTTPS
 * Then the master answers 201 with the account, a player, and where it lives
 */
TEST_F(AccountsRoutesFixture, CreatesAnAccountOverHttps) {
  const httplib::Result created = createAce();

  ASSERT_TRUE(created);
  EXPECT_EQ(created->status, EXPECTED_CREATED);
  EXPECT_EQ(memberIn(created, ROLE_MEMBER_NAME), PLAYER_ROLE_NAME);
  EXPECT_TRUE(created->get_header_value(std::string{LOCATION_HEADER_NAME})
                  .starts_with(USER_LOCATION_START));
}

/**
 * Given a master with its key of the passwords
 * When a client creates an account in plain HTTP, no proxy saying otherwise
 * Then the master refuses it with 403, so a password never crosses in clear
 */
TEST_F(AccountsRoutesFixture, RefusesAnAccountInPlainHttp) {
  RouteCall call = credentialsCall(USERS_URL, aceCredentials());
  call.overHttps = false;

  const httplib::Result refused = send(call);

  ASSERT_TRUE(refused);
  EXPECT_EQ(refused->status, EXPECTED_FORBIDDEN);
}

/**
 * Given the account ACE
 * When another account is created as "ace"
 * Then the master refuses it with 409: one nickname, whatever its case
 */
TEST_F(AccountsRoutesFixture, ANicknameNamesOneAccountWhateverItsCase) {
  ASSERT_TRUE(createAce());
  rtype::network::UserCredentials lowercase = aceCredentials();
  lowercase.nickname = std::string{ACE_IN_LOWERCASE};

  const httplib::Result refused = send(credentialsCall(USERS_URL, lowercase));

  ASSERT_TRUE(refused);
  EXPECT_EQ(refused->status, EXPECTED_CONFLICT);
}

/**
 * Given a master with its key of the passwords
 * When ACE creates an account with a password of seven bytes
 * Then the master refuses it with 400, naming the password
 */
TEST_F(AccountsRoutesFixture, RefusesAPasswordShorterThanEightBytes) {
  const httplib::Result refused =
      send(credentialsCall(USERS_URL, aceWith(SEVEN_BYTE_PASSWORD)));

  ASSERT_TRUE(refused);
  EXPECT_EQ(refused->status, EXPECTED_BAD_REQUEST);
  EXPECT_NE(refused->body.find(PASSWORD_POINTER_TEXT), std::string::npos);
}

/**
 * Given one address that created as many accounts as it may in an hour
 * When it creates one more
 * Then the master refuses it with 429
 */
TEST_F(AccountsRoutesFixture, LimitsTheAccountsOneAddressCreates) {
  for (std::size_t index = 0; index < ALLOWED_ACCOUNT_CREATIONS; ++index) {
    rtype::network::UserCredentials credentials = aceCredentials();
    credentials.nickname += std::to_string(index);
    const httplib::Result created =
        send(credentialsCall(USERS_URL, credentials));
    ASSERT_TRUE(created);
    ASSERT_EQ(created->status, EXPECTED_CREATED);
  }

  const httplib::Result refused = createAce();

  ASSERT_TRUE(refused);
  EXPECT_EQ(refused->status, EXPECTED_TOO_MANY_REQUESTS);
}

/**
 * Given the account ACE
 * When ACE signs in with its password
 * Then the master answers 201 with a session token, shown this once
 */
TEST_F(AccountsRoutesFixture, OpensASessionWithTheRightPassword) {
  ASSERT_TRUE(createAce());

  const httplib::Result opened = signInAsAce(ACE_PASSWORD);

  ASSERT_TRUE(opened);
  EXPECT_EQ(opened->status, EXPECTED_CREATED);
  EXPECT_TRUE(
      memberIn(opened, TOKEN_MEMBER_NAME).starts_with(SESSION_TOKEN_START));
}

/**
 * Given the account ACE
 * When someone signs in as ACE with another password
 * Then the master refuses it with 401
 */
TEST_F(AccountsRoutesFixture, RefusesAWrongPassword) {
  ASSERT_TRUE(createAce());

  const httplib::Result refused = signInAsAce(WRONG_PASSWORD);

  ASSERT_TRUE(refused);
  EXPECT_EQ(refused->status, EXPECTED_UNAUTHORIZED);
}

/**
 * Given an address whose sign-ins as ACE failed as often as the master allows
 * When it signs in once more, with the right password this time
 * Then the master refuses it with 429 before it checks the password
 */
TEST_F(AccountsRoutesFixture, LimitsTheFailedSignInsOfAnAddress) {
  ASSERT_TRUE(createAce());
  for (std::size_t index = 0; index < ALLOWED_SIGN_IN_FAILURES; ++index) {
    const httplib::Result failed = signInAsAce(WRONG_PASSWORD);
    ASSERT_TRUE(failed);
    ASSERT_EQ(failed->status, EXPECTED_UNAUTHORIZED);
  }

  const httplib::Result refused = signInAsAce(ACE_PASSWORD);

  ASSERT_TRUE(refused);
  EXPECT_EQ(refused->status, EXPECTED_TOO_MANY_REQUESTS);
}

/**
 * Given the account ACE
 * When someone signs in as ACE with a wrong password
 * Then the journal names the refused sign-in
 */
TEST_F(AccountsRoutesFixture, WritesARefusedSignInInTheJournal) {
  ASSERT_TRUE(createAce());

  ASSERT_TRUE(signInAsAce(WRONG_PASSWORD));

  EXPECT_FALSE(master().journal().linesHolding(REFUSED_SIGN_IN_TEXT).empty());
}

/**
 * Given the account ACE
 * When it is created, then signed in as with a wrong and the right password
 * Then no line of the journal holds either password
 */
TEST_F(AccountsRoutesFixture, NeverWritesAPasswordInTheJournal) {
  ASSERT_TRUE(createAce());

  ASSERT_TRUE(signInAsAce(WRONG_PASSWORD));
  ASSERT_TRUE(signInAsAce(ACE_PASSWORD));

  EXPECT_TRUE(master().journal().linesHolding(WRONG_PASSWORD).empty());
  EXPECT_TRUE(master().journal().linesHolding(ACE_PASSWORD).empty());
}

/**
 * Given a session of ACE
 * When its token reads its account
 * Then the master answers 200 with ACE
 */
TEST_F(AccountsRoutesFixture, TheTokenReadsItsAccount) {
  ASSERT_TRUE(createAce());
  const std::string token =
      memberIn(signInAsAce(ACE_PASSWORD), TOKEN_MEMBER_NAME);

  const httplib::Result account = readOwnUser(token);

  ASSERT_TRUE(account);
  EXPECT_EQ(account->status, EXPECTED_OK);
  EXPECT_EQ(memberIn(account, USER_NICKNAME_MEMBER_NAME), ACE_NICKNAME);
}

/**
 * Given a session of ACE that ACE closed
 * When its token reads its account
 * Then the master answers 401
 */
TEST_F(AccountsRoutesFixture, AClosedSessionReadsNoAccount) {
  ASSERT_TRUE(createAce());
  const std::string token =
      memberIn(signInAsAce(ACE_PASSWORD), TOKEN_MEMBER_NAME);
  const httplib::Result closed = deleteSession(token);
  ASSERT_TRUE(closed);
  ASSERT_EQ(closed->status, EXPECTED_NO_CONTENT);

  const httplib::Result account = readOwnUser(token);

  ASSERT_TRUE(account);
  EXPECT_EQ(account->status, EXPECTED_UNAUTHORIZED);
}

/**
 * Given a session of ACE that ACE closed
 * When ACE closes it again
 * Then the master answers 204: closing what is gone succeeds
 */
TEST_F(AccountsRoutesFixture, ClosingAClosedSessionSucceeds) {
  ASSERT_TRUE(createAce());
  const std::string token =
      memberIn(signInAsAce(ACE_PASSWORD), TOKEN_MEMBER_NAME);
  ASSERT_TRUE(deleteSession(token));

  const httplib::Result closedAgain = deleteSession(token);

  ASSERT_TRUE(closedAgain);
  EXPECT_EQ(closedAgain->status, EXPECTED_NO_CONTENT);
}

/**
 * Given a master launched without a key of the passwords
 * When it starts
 * Then it stops before its port opens, naming the missing key
 */
TEST(AccountsRoutes, AMasterWithoutKeyDoesNotStart) {
  EXPECT_THROW(
      const RunningMaster master(std::vector<std::string>{}, std::string{}),
      rtype::master::MissingPasswordKeyException);
}

/**
 * Given a master launched with a key one byte short
 * When it starts
 * Then it stops before its port opens
 */
TEST(AccountsRoutes, AMasterWithAKeyThatIsNoKeyDoesNotStart) {
  EXPECT_THROW(
      const RunningMaster master(std::vector<std::string>{},
                                 std::string{PASSWORD_KEY_ONE_BYTE_SHORT}),
      rtype::master::InvalidPasswordKeyException);
}

/**
 * Given a master launched without a key, whose master.cfg turns its accounts
 * off
 * When it starts and a client creates an account over HTTPS
 * Then it runs, and no route of the accounts answers
 */
TEST(AccountsRoutes, AMasterWithItsAccountsOffStartsWithoutKey) {
  const RunningMaster master(accountsOffLines(), std::string{});

  const httplib::Result refused = sendRouteCall(
      master.port(), credentialsCall(USERS_URL, aceCredentials()));

  ASSERT_TRUE(refused);
  EXPECT_EQ(refused->status, EXPECTED_NOT_FOUND);
}

/**
 * Given a master.cfg holding `admin ACE`, and the account ACE created
 * When the master starts again and ACE reads its account
 * Then ACE is an admin
 */
TEST(AccountsRoutes, AnAdminLineMakesTheAccountAnAdmin) {
  RunningMaster master{std::vector<std::string>{std::string{ACE_ADMIN_LINE}}};
  ASSERT_TRUE(sendRouteCall(master.port(),
                            credentialsCall(USERS_URL, aceCredentials())));
  master.restart();
  const std::string token =
      memberIn(sendRouteCall(master.port(),
                             credentialsCall(SESSIONS_URL, aceCredentials())),
               TOKEN_MEMBER_NAME);

  const httplib::Result account =
      sendRouteCall(master.port(), readOwnUserCall(token));

  EXPECT_EQ(memberIn(account, ROLE_MEMBER_NAME), ADMIN_ROLE_NAME);
}
