#include <gtest/gtest.h>
#include <optional>
#include <string>
#include "CannedAnswer.hpp"
#include "ExchangeReceipt.hpp"
#include "LookPurchase.hpp"
#include "MasterAccountLinkFixture.hpp"
#include "MasterAccountLinkFixtures.hpp"
#include "MasterAccountLinkTestConstants.hpp"
#include "MasterMessageText.hpp"
#include "SignInRefusal.hpp"
#include "UserReading.hpp"
#include "WalletChangeReading.hpp"
#include "WalletExchange.hpp"
#include "WalletReading.hpp"
#include "WalletRefusal.hpp"

using rtype::client::SignInRefusal;
using rtype::client::UserReading;
using rtype::client::WalletChangeReading;
using rtype::client::WalletReading;
using rtype::client::WalletRefusal;
using rtype::client::testing::BUY_CROWN_ROUTE;
using rtype::client::testing::LINK_BUY_500_CHIPS;
using rtype::client::testing::LINK_CONFLICT_STATUS;
using rtype::client::testing::LINK_CREATED_STATUS;
using rtype::client::testing::LINK_CROWN;
using rtype::client::testing::LINK_MADE_AT;
using rtype::client::testing::LINK_OK_STATUS;
using rtype::client::testing::LINK_SERVER_ERROR_STATUS;
using rtype::client::testing::LINK_TOKEN;
using rtype::client::testing::LINK_UNAUTHORIZED_STATUS;
using rtype::client::testing::LINK_UNPROCESSABLE_STATUS;
using rtype::client::testing::LINK_WALLET;
using rtype::client::testing::LINK_WALLET_AFTER;
using rtype::client::testing::linkAce;
using rtype::client::testing::linkLooks;
using rtype::client::testing::MasterAccountLinkFixture;
using rtype::client::testing::OWN_EXCHANGES_ROUTE;
using rtype::client::testing::OWN_LOOKS_ROUTE;
using rtype::client::testing::OWN_USER_ROUTE;
using rtype::client::testing::OWN_WALLET_ROUTE;
using rtype::network::ExchangeReceipt;
using rtype::network::LookPurchase;
using rtype::network::WalletExchange;

/**
 * Given a master that knows the token as the session of ACE
 * When the link asks the account of the token
 * Then its reading names ACE
 */
TEST_F(MasterAccountLinkFixture, ReadsTheAccountOfTheToken) {
  master().answer(std::string{OWN_USER_ROUTE},
                  CannedAnswer{.status = LINK_OK_STATUS,
                               .body = rtype::network::userText(linkAce())});

  const std::optional<UserReading> reading =
      userSoon(link().askUser(std::string{LINK_TOKEN}));

  ASSERT_TRUE(reading.has_value());
  EXPECT_EQ(reading.value_or(UserReading{}).user, linkAce());
}

/**
 * Given a master that knows no session of the token
 * When the link asks the account of the token
 * Then its reading says the token was refused
 */
TEST_F(MasterAccountLinkFixture, SaysAnUnknownTokenWasRefused) {
  master().answer(std::string{OWN_USER_ROUTE},
                  CannedAnswer{.status = LINK_UNAUTHORIZED_STATUS, .body = {}});

  const std::optional<UserReading> reading =
      userSoon(link().askUser(std::string{LINK_TOKEN}));

  ASSERT_TRUE(reading.has_value());
  EXPECT_EQ(reading.value_or(UserReading{}).refusal,
            SignInRefusal::TokenRefused);
}

/**
 * Given a master that fails on the account route
 * When the link asks the account of a token
 * Then its reading says the master refused, not the token
 */
TEST_F(MasterAccountLinkFixture, TellsAFailingMasterFromARefusedToken) {
  master().answer(std::string{OWN_USER_ROUTE},
                  CannedAnswer{.status = LINK_SERVER_ERROR_STATUS, .body = {}});

  const std::optional<UserReading> reading =
      userSoon(link().askUser(std::string{LINK_TOKEN}));

  ASSERT_TRUE(reading.has_value());
  EXPECT_EQ(reading.value_or(UserReading{}).refusal,
            SignInRefusal::MasterRefused);
}

/**
 * Given a master that holds the wallet and the looks of the token
 * When the link asks the wallet
 * Then its reading holds both, with the token it was read with
 */
TEST_F(MasterAccountLinkFixture, ReadsTheBalancesAndTheLooks) {
  master().answer(
      std::string{OWN_WALLET_ROUTE},
      CannedAnswer{.status = LINK_OK_STATUS,
                   .body = rtype::network::walletText(LINK_WALLET)});
  master().answer(
      std::string{OWN_LOOKS_ROUTE},
      CannedAnswer{.status = LINK_OK_STATUS,
                   .body = rtype::network::ownedLooksText(linkLooks())});

  link().askWallet(std::string{LINK_TOKEN});

  const std::optional<WalletReading> reading = walletSoon();

  ASSERT_TRUE(reading.has_value());
  EXPECT_EQ(reading.value_or(WalletReading{}).wallet, LINK_WALLET);
  EXPECT_EQ(reading.value_or(WalletReading{}).looks, linkLooks());
  EXPECT_EQ(reading.value_or(WalletReading{}).token, LINK_TOKEN);
}

/**
 * Given a master that refuses the crown for lack of kopecks
 * When the link buys the crown
 * Then its answer says the balances refused it
 */
TEST_F(MasterAccountLinkFixture, SaysAPurchaseThePriceRefused) {
  master().answer(std::string{BUY_CROWN_ROUTE},
                  CannedAnswer{.status = LINK_CONFLICT_STATUS, .body = {}});

  const std::optional<WalletChangeReading> answer =
      answerSoon([this] { return link().purchase(); },
                 link().askPurchase(std::string{LINK_TOKEN}, LINK_CROWN));

  ASSERT_TRUE(answer.has_value());
  EXPECT_EQ(answer.value_or(WalletChangeReading{}).refusal,
            WalletRefusal::BalanceRefused);
}

/**
 * Given a master that sells the crown, whose wallet route still answers the
 * balances before it
 * When the link buys the crown, the wallet never asked
 * Then the wallet shows the balances the purchase answered, and the looks
 * read again
 */
TEST_F(MasterAccountLinkFixture, APurchaseMadeShowsTheBalancesItAnswered) {
  master().answer(std::string{BUY_CROWN_ROUTE},
                  CannedAnswer{.status = LINK_CREATED_STATUS,
                               .body = rtype::network::lookPurchaseText(
                                   LookPurchase{.item = std::string{LINK_CROWN},
                                                .wallet = LINK_WALLET_AFTER})});
  master().answer(
      std::string{OWN_WALLET_ROUTE},
      CannedAnswer{.status = LINK_OK_STATUS,
                   .body = rtype::network::walletText(LINK_WALLET)});
  master().answer(
      std::string{OWN_LOOKS_ROUTE},
      CannedAnswer{.status = LINK_OK_STATUS,
                   .body = rtype::network::ownedLooksText(linkLooks())});

  static_cast<void>(link().askPurchase(std::string{LINK_TOKEN}, LINK_CROWN));

  const std::optional<WalletReading> reading = walletSoon();
  ASSERT_TRUE(reading.has_value());
  EXPECT_EQ(reading.value_or(WalletReading{}).wallet, LINK_WALLET_AFTER);
  EXPECT_EQ(reading.value_or(WalletReading{}).looks, linkLooks());
}

/**
 * Given a master whose cashier changes 500 kopecks into chips
 * When the link asks that exchange
 * Then its answer holds the balances left
 */
TEST_F(MasterAccountLinkFixture, AnExchangeAnswersWithTheBalancesLeft) {
  master().answer(
      std::string{OWN_EXCHANGES_ROUTE},
      CannedAnswer{
          .status = LINK_CREATED_STATUS,
          .body = rtype::network::exchangeReceiptText(ExchangeReceipt{
              .exchange = WalletExchange{.id = 1,
                                         .from = LINK_BUY_500_CHIPS.from,
                                         .amount = LINK_BUY_500_CHIPS.amount,
                                         .madeAt = LINK_MADE_AT},
              .wallet = LINK_WALLET_AFTER})});

  const std::optional<WalletChangeReading> answer = answerSoon(
      [this] { return link().exchange(); },
      link().askExchange(std::string{LINK_TOKEN}, LINK_BUY_500_CHIPS));

  ASSERT_TRUE(answer.has_value());
  EXPECT_EQ(answer.value_or(WalletChangeReading{}).wallet, LINK_WALLET_AFTER);
}

/**
 * Given a master whose cashier refuses an amount of none with 422
 * When the link asks that exchange
 * Then its answer says the amount was refused, not a look
 */
TEST_F(MasterAccountLinkFixture, AnExchangeOfNoAmountIsRefusedForItsAmount) {
  master().answer(
      std::string{OWN_EXCHANGES_ROUTE},
      CannedAnswer{.status = LINK_UNPROCESSABLE_STATUS, .body = {}});

  const std::optional<WalletChangeReading> answer = answerSoon(
      [this] { return link().exchange(); },
      link().askExchange(std::string{LINK_TOKEN}, LINK_BUY_500_CHIPS));

  ASSERT_TRUE(answer.has_value());
  EXPECT_EQ(answer.value_or(WalletChangeReading{}).refusal,
            WalletRefusal::InvalidAmount);
}

/**
 * Given a master whose shop refuses the crown with 422
 * When the link buys the crown
 * Then its answer says the look is not for sale
 */
TEST_F(MasterAccountLinkFixture, APurchaseOfALookNotSoldSaysSo) {
  master().answer(
      std::string{BUY_CROWN_ROUTE},
      CannedAnswer{.status = LINK_UNPROCESSABLE_STATUS, .body = {}});

  const std::optional<WalletChangeReading> answer =
      answerSoon([this] { return link().purchase(); },
                 link().askPurchase(std::string{LINK_TOKEN}, LINK_CROWN));

  ASSERT_TRUE(answer.has_value());
  EXPECT_EQ(answer.value_or(WalletChangeReading{}).refusal,
            WalletRefusal::NotForSale);
}

/**
 * Given a wallet read, and a master whose cashier changes 500 kopecks into
 * chips while its wallet route still answers the balances before
 * When the link asks that exchange
 * Then the wallet shows the balances the exchange answered, its looks kept
 */
TEST_F(MasterAccountLinkFixture, AnExchangeMadeShowsTheBalancesItAnswered) {
  master().answer(
      std::string{OWN_WALLET_ROUTE},
      CannedAnswer{.status = LINK_OK_STATUS,
                   .body = rtype::network::walletText(LINK_WALLET)});
  master().answer(
      std::string{OWN_LOOKS_ROUTE},
      CannedAnswer{.status = LINK_OK_STATUS,
                   .body = rtype::network::ownedLooksText(linkLooks())});
  master().answer(
      std::string{OWN_EXCHANGES_ROUTE},
      CannedAnswer{
          .status = LINK_CREATED_STATUS,
          .body = rtype::network::exchangeReceiptText(ExchangeReceipt{
              .exchange = WalletExchange{.id = 1,
                                         .from = LINK_BUY_500_CHIPS.from,
                                         .amount = LINK_BUY_500_CHIPS.amount,
                                         .madeAt = LINK_MADE_AT},
              .wallet = LINK_WALLET_AFTER})});
  link().askWallet(std::string{LINK_TOKEN});
  ASSERT_TRUE(walletSoon().has_value());

  static_cast<void>(answerSoon(
      [this] { return link().exchange(); },
      link().askExchange(std::string{LINK_TOKEN}, LINK_BUY_500_CHIPS)));

  const std::optional<WalletReading> reading = link().wallet();
  ASSERT_TRUE(reading.has_value());
  EXPECT_EQ(reading.value_or(WalletReading{}).wallet, LINK_WALLET_AFTER);
  EXPECT_EQ(reading.value_or(WalletReading{}).looks, linkLooks());
}
