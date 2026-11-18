#include <gtest/gtest.h>
#include <string>
#include "CannedAnswer.hpp"
#include "LookPurchase.hpp"
#include "MasterAccountClientFixture.hpp"
#include "MasterAccountClientFixtures.hpp"
#include "MasterAccountClientTestConstants.hpp"
#include "MasterMessageText.hpp"
#include "NetworkException.hpp"

using rtype::network::exchangeReceiptText;
using rtype::network::exchangeRequestText;
using rtype::network::InvalidMasterMessageException;
using rtype::network::LookPurchase;
using rtype::network::lookPurchaseText;
using rtype::network::MasterRefusedException;
using rtype::network::ownedLooksText;
using rtype::network::userText;
using rtype::network::walletText;

/**
 * Given a master that knows the token as the session of ACE
 * When the client reads the account of the token
 * Then it reads ACE
 */
TEST_F(MasterAccountClientFixture, ReadsTheAccountOfTheToken) {
  master().answer(
      std::string{READ_OWN_USER},
      CannedAnswer{.status = OK_STATUS, .body = userText(aceUser())});

  EXPECT_EQ(client().readOwnUser(std::string{ACCOUNT_TOKEN}), aceUser());
}

/**
 * Given a master
 * When the client reads the account of a token
 * Then the token goes as Bearer
 */
TEST_F(MasterAccountClientFixture, SendsTheTokenAsBearer) {
  master().answer(
      std::string{READ_OWN_USER},
      CannedAnswer{.status = OK_STATUS, .body = userText(aceUser())});

  static_cast<void>(client().readOwnUser(std::string{ACCOUNT_TOKEN}));

  EXPECT_EQ(lastRequest().authorization, BEARER_ACCOUNT_TOKEN);
}

/**
 * Given a master that answers the account route with an object holding no
 * account
 * When the client reads the account of a token
 * Then it refuses the answer
 */
TEST_F(MasterAccountClientFixture, RefusesAnAnswerThatIsNoAccount) {
  master().answer(
      std::string{READ_OWN_USER},
      CannedAnswer{.status = OK_STATUS, .body = std::string{NO_ACCOUNT_TEXT}});

  EXPECT_THROW(
      static_cast<void>(client().readOwnUser(std::string{ACCOUNT_TOKEN})),
      InvalidMasterMessageException);
}

/**
 * Given a master that knows no session of the token
 * When the client reads the account of the token
 * Then the refusal carries 401
 */
TEST_F(MasterAccountClientFixture, CarriesTheRefusalOfAnUnknownToken) {
  master().answer(std::string{READ_OWN_USER},
                  CannedAnswer{.status = UNAUTHORIZED_STATUS, .body = {}});

  try {
    static_cast<void>(client().readOwnUser(std::string{ACCOUNT_TOKEN}));
    FAIL() << "the token was not refused";
  } catch (const MasterRefusedException& refusal) {
    EXPECT_EQ(refusal.status(), UNAUTHORIZED_STATUS);
  }
}

/**
 * Given a master that holds the wallet of ACE
 * When the client reads the wallet of the token
 * Then it reads its balances
 */
TEST_F(MasterAccountClientFixture, ReadsTheBalancesOfTheWallet) {
  master().answer(
      std::string{READ_OWN_WALLET},
      CannedAnswer{.status = OK_STATUS, .body = walletText(ACE_WALLET)});

  EXPECT_EQ(client().readOwnWallet(std::string{ACCOUNT_TOKEN}), ACE_WALLET);
}

/**
 * Given a master that holds the looks of ACE
 * When the client lists the looks of the token
 * Then it reads them
 */
TEST_F(MasterAccountClientFixture, ListsTheLooksOfTheAccount) {
  master().answer(
      std::string{LIST_OWN_LOOKS},
      CannedAnswer{.status = OK_STATUS, .body = ownedLooksText(aceLooks())});

  EXPECT_EQ(client().listOwnLooks(std::string{ACCOUNT_TOKEN}), aceLooks());
}

/**
 * Given a master that sells the crown
 * When the client buys the crown
 * Then it puts the path of the crown, and reads the look and the balances
 * left
 */
TEST_F(MasterAccountClientFixture, BuysALookAtThePathOfItsName) {
  const LookPurchase bought{.item = std::string{CROWN}, .wallet = WALLET_AFTER};
  master().answer(
      std::string{BUY_CROWN},
      CannedAnswer{.status = CREATED_STATUS, .body = lookPurchaseText(bought)});

  EXPECT_EQ(client().buyOwnLook(std::string{ACCOUNT_TOKEN}, CROWN), bought);
  EXPECT_EQ(lastRequest().path, CROWN_PATH);
}

/**
 * Given a master that refuses the crown for lack of kopecks
 * When the client buys the crown
 * Then the refusal carries 409
 */
TEST_F(MasterAccountClientFixture, CarriesTheRefusalOfAPurchase) {
  master().answer(std::string{BUY_CROWN},
                  CannedAnswer{.status = CONFLICT_STATUS, .body = {}});

  try {
    static_cast<void>(client().buyOwnLook(std::string{ACCOUNT_TOKEN}, CROWN));
    FAIL() << "the purchase was not refused";
  } catch (const MasterRefusedException& refusal) {
    EXPECT_EQ(refusal.status(), CONFLICT_STATUS);
  }
}

/**
 * Given a master whose cashier changes 500 kopecks into chips
 * When the client asks for that exchange
 * Then it sends the request in the body, and reads the receipt
 */
TEST_F(MasterAccountClientFixture, SendsAnExchangeAndReadsItsReceipt) {
  master().answer(
      std::string{CREATE_OWN_EXCHANGE},
      CannedAnswer{.status = CREATED_STATUS,
                   .body = exchangeReceiptText(buyFiveHundredChipsReceipt())});

  EXPECT_EQ(client().exchange(std::string{ACCOUNT_TOKEN}, BUY_500_CHIPS),
            buyFiveHundredChipsReceipt());
  EXPECT_EQ(lastRequest().body, exchangeRequestText(BUY_500_CHIPS));
}
