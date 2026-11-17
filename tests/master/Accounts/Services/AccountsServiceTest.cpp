#include <gtest/gtest.h>
#include <string>
#include "AccountsServiceFixture.hpp"
#include "AccountsServiceTestConstants.hpp"
#include "AccountsTestConstants.hpp"

/**
 * Given a session of ACE opened at a date
 * When its token is read a second before a week has passed
 * Then it still names ACE
 */
TEST_F(AccountsServiceFixture, ASessionLivesAWeek) {
  const std::string token = aceToken();

  EXPECT_TRUE(accountAt(token, JUST_BEFORE_A_WEEK).has_value());
}

/**
 * Given a session of ACE opened at a date and never used
 * When its token is read a week later
 * Then it names no account: the session expired
 */
TEST_F(AccountsServiceFixture, AnUnusedSessionExpiresAfterAWeek) {
  const std::string token = aceToken();

  EXPECT_FALSE(accountAt(token, A_WEEK_LATER).has_value());
}

/**
 * Given a session of ACE used two days after it opened
 * When its token is read eight days after it opened
 * Then it still names ACE: a use moves its expiry a week after the use
 */
TEST_F(AccountsServiceFixture, AUseMovesTheExpiryAWeekLater) {
  const std::string token = aceToken();
  ASSERT_TRUE(accountAt(token, TWO_DAYS_LATER).has_value());

  EXPECT_TRUE(accountAt(token, EIGHT_DAYS_LATER).has_value());
}

/**
 * Given a session of ACE used half a day after it opened
 * When its token is read a week after it opened
 * Then it names no account: a use within a day of the last move writes
 * nothing
 */
TEST_F(AccountsServiceFixture, AUseWithinADayMovesNothing) {
  const std::string token = aceToken();
  ASSERT_TRUE(accountAt(token, HALF_A_DAY_LATER).has_value());

  EXPECT_FALSE(accountAt(token, A_WEEK_LATER).has_value());
}

/**
 * Given the account ACE
 * When the database is read as a thief would read it
 * Then it keeps 128 bytes for the password, and never the password itself
 */
TEST_F(AccountsServiceFixture, KeepsNoPasswordInClear) {
  const std::string kept = keptPassword();

  EXPECT_EQ(kept.size(), KEPT_PASSWORD_BYTES);
  EXPECT_EQ(kept.find(ACE_PASSWORD), std::string::npos);
}
