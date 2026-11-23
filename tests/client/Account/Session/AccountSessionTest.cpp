#include <gtest/gtest.h>
#include <optional>
#include <string>
#include "AccountFixtures.hpp"
#include "AccountLinkConstants.hpp"
#include "AccountSessionFixture.hpp"
#include "AccountTestConstants.hpp"
#include "SignInRefusal.hpp"
#include "UserReading.hpp"

using rtype::client::NO_ACCOUNT_ID;
using rtype::client::SignInRefusal;
using rtype::client::UserReading;
using rtype::client::testing::AccountSessionFixture;
using rtype::client::testing::ACE_ACCOUNT_ID;
using rtype::client::testing::ACE_ACCOUNT_NICKNAME;
using rtype::client::testing::ACE_TOKEN;
using rtype::client::testing::aceAccount;
using rtype::client::testing::FIRST_ASK;
using rtype::client::testing::OTHER_TOKEN;

/**
 * Given a session no token signed in
 * When its account is read
 * Then the player is a guest
 */
TEST_F(AccountSessionFixture, ANewSessionIsAGuest) {
  EXPECT_FALSE(session().signedIn());
  EXPECT_EQ(session().accountId(), NO_ACCOUNT_ID);
}

/**
 * Given a session
 * When it signs in with a token
 * Then it asks the master the account of that token
 */
TEST_F(AccountSessionFixture, SignInAsksTheAccountOfTheToken) {
  session().signIn(std::string{ACE_TOKEN});

  EXPECT_EQ(link().asked.userToken, ACE_TOKEN);
}

/**
 * Given a session that signed in with a token
 * When the master has not answered yet
 * Then the player is still a guest
 */
TEST_F(AccountSessionFixture, IsAGuestUntilTheMasterAnswers) {
  session().signIn(std::string{ACE_TOKEN});

  EXPECT_EQ(session().accountId(), NO_ACCOUNT_ID);
}

/**
 * Given a session that signed in with the token of ACE
 * When the master names ACE
 * Then the player plays under the account of ACE, with its token
 */
TEST_F(AccountSessionFixture, PlaysUnderTheAccountTheMasterNames) {
  signInAsAce();

  EXPECT_EQ(session().accountId(), ACE_ACCOUNT_ID);
  EXPECT_EQ(session().nickname(), ACE_ACCOUNT_NICKNAME);
  EXPECT_EQ(session().token(), ACE_TOKEN);
}

/**
 * Given a session that signed in with a token, then with another
 * When the master names an account for the first one
 * Then that answer signs no one in
 */
TEST_F(AccountSessionFixture, AnAnswerToAnOlderTokenIsNotTaken) {
  session().signIn(std::string{ACE_TOKEN});
  session().signIn(std::string{OTHER_TOKEN});

  link().userRead =
      UserReading{.ask = FIRST_ASK, .user = aceAccount(), .refusal = {}};

  EXPECT_FALSE(session().signedIn());
}

/**
 * Given a session that signed in with a token
 * When the master refuses the token
 * Then the player is a guest, and the session says why
 */
TEST_F(AccountSessionFixture, ARefusedTokenSignsNoOneIn) {
  session().signIn(std::string{ACE_TOKEN});

  link().userRead = UserReading{
      .ask = FIRST_ASK, .user = {}, .refusal = SignInRefusal::TokenRefused};

  EXPECT_EQ(session().accountId(), NO_ACCOUNT_ID);
  EXPECT_EQ(session().signInRefusal(), SignInRefusal::TokenRefused);
}

/**
 * Given a session signed in to ACE
 * When it signs out
 * Then the player is a guest, and no token is left
 */
TEST_F(AccountSessionFixture, SignOutForgetsTheAccountAndTheToken) {
  signInAsAce();

  session().signOut();

  EXPECT_EQ(session().accountId(), NO_ACCOUNT_ID);
  EXPECT_TRUE(session().token().empty());
}

/**
 * Given a session that signed in with the token of ACE
 * When the master names ACE
 * Then the token is kept for the next launch
 */
TEST_F(AccountSessionFixture, KeepsTheTokenOnceTheMasterNamesItsAccount) {
  signInAsAce();

  session().rememberAnswer();

  EXPECT_EQ(storage().kept, std::optional<std::string>{ACE_TOKEN});
}

/**
 * Given a session that signed in with a token
 * When the master has not answered yet
 * Then no token is kept
 */
TEST_F(AccountSessionFixture, KeepsNoTokenBeforeTheMasterAnswers) {
  session().signIn(std::string{ACE_TOKEN});

  session().rememberAnswer();

  EXPECT_FALSE(storage().kept.has_value());
}

/**
 * Given a session signed in to ACE whose token was kept
 * When frames go on with the same answer
 * Then the token is not written again
 */
TEST_F(AccountSessionFixture, WritesTheKeptTokenOnce) {
  signInAsAce();
  session().rememberAnswer();

  session().rememberAnswer();

  EXPECT_EQ(storage().keeps, 1);
}

/**
 * Given a session signed in to ACE whose token was kept
 * When it signs out
 * Then the next launch finds no token
 */
TEST_F(AccountSessionFixture, SignOutForgetsTheKeptToken) {
  signInAsAce();
  session().rememberAnswer();

  session().signOut();

  EXPECT_FALSE(storage().kept.has_value());
}

/**
 * Given the token of ACE kept by the previous launch
 * When the session restores it
 * Then it asks the master the account of that token
 */
TEST_F(AccountSessionFixture, RestoreSignsInWithTheKeptToken) {
  storage().kept = std::string{ACE_TOKEN};

  session().restore();

  EXPECT_EQ(link().asked.userToken, ACE_TOKEN);
}

/**
 * Given no token kept by the previous launch
 * When the session restores
 * Then it asks the master nothing, and the player is a guest
 */
TEST_F(AccountSessionFixture, RestoreWithNoKeptTokenAsksNothing) {
  session().restore();

  EXPECT_TRUE(link().asked.userToken.empty());
}

/**
 * Given the token of ACE kept by the previous launch, restored
 * When the master names ACE
 * Then the token is not written again
 */
TEST_F(AccountSessionFixture, ARestoredTokenIsNotWrittenAgain) {
  storage().kept = std::string{ACE_TOKEN};
  session().restore();
  link().userRead =
      UserReading{.ask = FIRST_ASK, .user = aceAccount(), .refusal = {}};

  session().rememberAnswer();

  EXPECT_EQ(storage().keeps, 0);
}

/**
 * Given the token of ACE kept by the previous launch, restored
 * When the master refuses it, unknown or expired
 * Then the token is forgotten
 */
TEST_F(AccountSessionFixture, ForgetsAKeptTokenTheMasterRefuses) {
  storage().kept = std::string{ACE_TOKEN};
  session().restore();
  link().userRead = UserReading{
      .ask = FIRST_ASK, .user = {}, .refusal = SignInRefusal::TokenRefused};

  session().rememberAnswer();

  EXPECT_FALSE(storage().kept.has_value());
}

/**
 * Given the token of ACE kept by the previous launch, restored
 * When the master cannot answer
 * Then the token stays kept, for the next launch to try again
 */
TEST_F(AccountSessionFixture, KeepsTheTokenWhenTheMasterCannotAnswer) {
  storage().kept = std::string{ACE_TOKEN};
  session().restore();
  link().userRead = UserReading{
      .ask = FIRST_ASK, .user = {}, .refusal = SignInRefusal::MasterRefused};

  session().rememberAnswer();

  EXPECT_EQ(storage().kept, std::optional<std::string>{ACE_TOKEN});
}
