#include <gtest/gtest.h>
#include <optional>
#include <string>
#include "AccountLinkConstants.hpp"
#include "AccountTestConstants.hpp"
#include "AccountWalletFixture.hpp"
#include "ShipLocker.hpp"
#include "WalletChangeReading.hpp"

using rtype::client::NO_ACCOUNT_ASK;
using rtype::client::ShipLocker;
using rtype::client::WalletChangeReading;
using rtype::client::testing::AccountWalletFixture;
using rtype::client::testing::ACE_ACCOUNT_ID;
using rtype::client::testing::ACE_BALANCES;
using rtype::client::testing::ACE_TOKEN;
using rtype::client::testing::CROWN_ITEM;
using rtype::client::testing::CROWN_NAME;
using rtype::client::testing::FIRST_ASK;
using rtype::client::testing::FIVE_HUNDRED_CHIPS;
using rtype::client::testing::OTHER_TOKEN;
using rtype::client::testing::SECOND_ASK;

/**
 * Given a wallet the link read, the player signed in with its token
 * When its balances are read
 * Then they are the balances the master answered
 */
TEST_F(AccountWalletFixture, ShowsTheBalancesReadWithTheTokenSignedIn) {
  readAceWalletWith(std::string{ACE_TOKEN});

  EXPECT_EQ(wallet().wallet(), ACE_BALANCES);
}

/**
 * Given a wallet the link read, then the player signed out
 * When its balances are read
 * Then there are none
 */
TEST_F(AccountWalletFixture, SignedOutItHoldsNothing) {
  readAceWalletWith(std::string{ACE_TOKEN});

  session().signOut();

  EXPECT_FALSE(wallet().wallet().has_value());
}

/**
 * Given a wallet the link read with another token than the one signed in
 * When its balances are read
 * Then there are none: they belong to another session
 */
TEST_F(AccountWalletFixture, AReadingOfAnotherTokenIsNotShown) {
  readAceWalletWith(std::string{OTHER_TOKEN});

  EXPECT_FALSE(wallet().wallet().has_value());
}

/**
 * Given a wallet that owns the crown alone
 * When its looks are read
 * Then the locker holds the crown, and nothing else
 */
TEST_F(AccountWalletFixture, TheLockerHoldsTheLooksTheMasterNames) {
  readAceWalletWith(std::string{ACE_TOKEN});

  ShipLocker crownOnly;
  crownOnly.items.reset();
  crownOnly.items.set(CROWN_ITEM);
  EXPECT_EQ(wallet().ownedLooks(), crownOnly);
}

/**
 * Given the player signed in
 * When the wallet buys the crown
 * Then the link asks the purchase of the crown, by its name, with the token
 */
TEST_F(AccountWalletFixture, BuysALookByItsName) {
  session().signInAs(ACE_ACCOUNT_ID, std::string{ACE_TOKEN});

  static_cast<void>(wallet().buy(CROWN_ITEM));

  EXPECT_EQ(link().asked.purchaseItem, CROWN_NAME);
  EXPECT_EQ(link().asked.token, ACE_TOKEN);
}

/**
 * Given the player signed out
 * When the wallet buys the crown
 * Then nothing is asked
 */
TEST_F(AccountWalletFixture, SignedOutAPurchaseAsksNothing) {
  EXPECT_EQ(wallet().buy(CROWN_ITEM), NO_ACCOUNT_ASK);
  EXPECT_EQ(link().asked.purchaseAsk, NO_ACCOUNT_ASK);
}

/**
 * Given the link answered the second purchase
 * When the answer to the first one is read
 * Then there is none
 */
TEST_F(AccountWalletFixture, AnAnswerToAnotherPurchaseIsNotTaken) {
  link().purchaseRead = WalletChangeReading{
      .ask = SECOND_ASK, .wallet = ACE_BALANCES, .refusal = {}};

  EXPECT_FALSE(wallet().purchase(FIRST_ASK).has_value());
}

/**
 * Given the player signed in
 * When the wallet asks an exchange
 * Then the link asks the cashier for it, with the token
 */
TEST_F(AccountWalletFixture, AsksTheCashierWithTheToken) {
  session().signInAs(ACE_ACCOUNT_ID, std::string{ACE_TOKEN});

  static_cast<void>(wallet().exchange(FIVE_HUNDRED_CHIPS));

  EXPECT_EQ(link().asked.exchangeRequest, FIVE_HUNDRED_CHIPS);
  EXPECT_EQ(link().asked.token, ACE_TOKEN);
}

/**
 * Given the session signs in
 * When the wallet follows it
 * Then the wallet of the account is asked, with its token
 */
TEST_F(AccountWalletFixture, AsksTheWalletOnceTheSessionSignsIn) {
  session().signInAs(ACE_ACCOUNT_ID, std::string{ACE_TOKEN});

  wallet().followSession();

  EXPECT_EQ(link().asked.walletAsk, FIRST_ASK);
  EXPECT_EQ(link().asked.token, ACE_TOKEN);
}

/**
 * Given the session signed in, and the wallet followed it once
 * When it follows it again, the account unchanged
 * Then nothing more is asked
 */
TEST_F(AccountWalletFixture, AsksTheWalletOncePerSignIn) {
  session().signInAs(ACE_ACCOUNT_ID, std::string{ACE_TOKEN});
  wallet().followSession();

  wallet().followSession();

  EXPECT_EQ(link().asked.walletAsk, FIRST_ASK);
}
