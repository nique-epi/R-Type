#include <gtest/gtest.h>
#include <string>
#include "AccountFixtures.hpp"
#include "AccountSessionFixture.hpp"
#include "AccountTestConstants.hpp"
#include "ProtocolConstants.hpp"
#include "SignInRefusal.hpp"
#include "UserReading.hpp"

using rtype::client::SignInRefusal;
using rtype::client::UserReading;
using rtype::client::testing::AccountSessionFixture;
using rtype::client::testing::ACE_ACCOUNT_ID;
using rtype::client::testing::ACE_ACCOUNT_NICKNAME;
using rtype::client::testing::ACE_TOKEN;
using rtype::client::testing::aceAccount;
using rtype::client::testing::FIRST_ASK;
using rtype::client::testing::OTHER_TOKEN;
using rtype::network::GUEST_ACCOUNT_ID;

/**
 * Given a session no token signed in
 * When its account is read
 * Then the player is a guest
 */
TEST_F(AccountSessionFixture, ANewSessionIsAGuest) {
  EXPECT_FALSE(session().signedIn());
  EXPECT_EQ(session().accountId(), GUEST_ACCOUNT_ID);
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

  EXPECT_EQ(session().accountId(), GUEST_ACCOUNT_ID);
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

  EXPECT_EQ(session().accountId(), GUEST_ACCOUNT_ID);
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

  EXPECT_EQ(session().accountId(), GUEST_ACCOUNT_ID);
  EXPECT_TRUE(session().token().empty());
}
