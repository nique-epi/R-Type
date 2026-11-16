#include <gtest/gtest.h>
#include <httplib.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "AccountsRoutesFixtures.hpp"
#include "AccountsRoutesTestConstants.hpp"
#include "MasterTestConstants.hpp"
#include "RouteCall.hpp"
#include "RouteCallFixtures.hpp"
#include "RunningMaster.hpp"
#include "ServersRoutesTestConstants.hpp"
#include "WalletRoutesFixture.hpp"
#include "WalletRoutesFixtures.hpp"
#include "WalletRoutesTestConstants.hpp"

/**
 * Given a master whose wallets open with 1000 kopecks, chips left at their
 * default
 * When ACE reads its wallet over HTTPS with the token of its session
 * Then the master answers 200 with 1000 kopecks and 500 chips
 */
TEST_F(WalletRoutesFixture, ReadsTheWalletOfTheSession) {
  const httplib::Result wallet =
      send(ownCall(GET_METHOD_TEXT, OWN_WALLET_URL, token()));

  ASSERT_TRUE(wallet);
  EXPECT_EQ(wallet->status, EXPECTED_OK);
  EXPECT_EQ(numberIn(wallet, KOPECKS_MEMBER_NAME),
            std::optional{STARTING_KOPECKS_SET});
  EXPECT_EQ(numberIn(wallet, CHIPS_MEMBER_NAME),
            std::optional{DEFAULT_STARTING_CHIPS_READ});
}

/**
 * Given a master with wallets
 * When a client reads a wallet without a token
 * Then the master answers 401
 */
TEST_F(WalletRoutesFixture, RefusesAWalletWithoutASession) {
  const httplib::Result refused =
      send(ownCall(GET_METHOD_TEXT, OWN_WALLET_URL, {}));

  ASSERT_TRUE(refused);
  EXPECT_EQ(refused->status, EXPECTED_UNAUTHORIZED);
}

/**
 * Given a master with wallets
 * When ACE reads its wallet in plain HTTP
 * Then the master answers 403, so a token never crosses in clear
 */
TEST_F(WalletRoutesFixture, RefusesAWalletInPlainHttp) {
  RouteCall call = ownCall(GET_METHOD_TEXT, OWN_WALLET_URL, token());
  call.overHttps = false;

  const httplib::Result refused = send(call);

  ASSERT_TRUE(refused);
  EXPECT_EQ(refused->status, EXPECTED_FORBIDDEN);
}

/**
 * Given a wallet of 1000 kopecks
 * When ACE changes 200 kopecks into chips, then reads where the answer says
 * the exchange lives
 * Then the master answers 201 with its place, and the exchange reads back
 */
TEST_F(WalletRoutesFixture, AnExchangeIsCreatedAndReadBack) {
  const httplib::Result created =
      send(exchangeCall(token(), std::string{TWO_HUNDRED_KOPECKS_BODY}));
  ASSERT_TRUE(created);
  ASSERT_EQ(created->status, EXPECTED_CREATED);
  const std::string location =
      created->get_header_value(std::string{LOCATION_HEADER_NAME});
  EXPECT_TRUE(location.starts_with(EXCHANGE_LOCATION_START));

  const httplib::Result read =
      send(ownCall(GET_METHOD_TEXT, location, token()));

  ASSERT_TRUE(read);
  EXPECT_EQ(read->status, EXPECTED_OK);
  EXPECT_EQ(numberIn(read, AMOUNT_MEMBER_NAME),
            std::optional{TWO_HUNDRED_EXCHANGED});
}

/**
 * Given a wallet of 1000 kopecks
 * When ACE changes 1001 kopecks into chips
 * Then the master answers 409
 */
TEST_F(WalletRoutesFixture, RefusesAnExchangePastTheBalance) {
  const httplib::Result refused =
      send(exchangeCall(token(), std::string{TOO_MANY_KOPECKS_BODY}));

  ASSERT_TRUE(refused);
  EXPECT_EQ(refused->status, EXPECTED_CONFLICT);
}

/**
 * Given a wallet
 * When ACE changes no chips into kopecks
 * Then the master answers 422
 */
TEST_F(WalletRoutesFixture, RefusesAnExchangeOfNone) {
  const httplib::Result refused =
      send(exchangeCall(token(), std::string{NO_AMOUNT_BODY}));

  ASSERT_TRUE(refused);
  EXPECT_EQ(refused->status, EXPECTED_UNPROCESSABLE_CONTENT);
}

/**
 * Given a wallet
 * When ACE asks the cashier for a currency it does not know
 * Then the master answers 400
 */
TEST_F(WalletRoutesFixture, RefusesAnExchangeOfAnUnknownCurrency) {
  const httplib::Result refused =
      send(exchangeCall(token(), std::string{GOLD_BODY}));

  ASSERT_TRUE(refused);
  EXPECT_EQ(refused->status, EXPECTED_BAD_REQUEST);
}

/**
 * Given a wallet that made no exchange
 * When ACE reads an exchange of a number it never made
 * Then the master answers 404
 */
TEST_F(WalletRoutesFixture, RefusesAnExchangeTheAccountNeverMade) {
  const httplib::Result refused =
      send(ownCall(GET_METHOD_TEXT, UNKNOWN_EXCHANGE_URL, token()));

  ASSERT_TRUE(refused);
  EXPECT_EQ(refused->status, EXPECTED_NOT_FOUND);
}

/**
 * Given a wallet of 1000 kopecks
 * When ACE buys the party hat twice
 * Then the master answers 201, then 200, and took 150 kopecks once
 */
TEST_F(WalletRoutesFixture, BuyingALookTwiceTakesItsPriceOnce) {
  const httplib::Result bought =
      send(ownCall(PUT_METHOD, PARTY_HAT_URL, token()));
  const httplib::Result again =
      send(ownCall(PUT_METHOD, PARTY_HAT_URL, token()));
  const httplib::Result wallet =
      send(ownCall(GET_METHOD_TEXT, OWN_WALLET_URL, token()));

  ASSERT_TRUE(bought);
  ASSERT_TRUE(again);
  EXPECT_EQ(bought->status, EXPECTED_CREATED);
  EXPECT_EQ(again->status, EXPECTED_OK);
  EXPECT_EQ(numberIn(wallet, KOPECKS_MEMBER_NAME),
            std::optional{KOPECKS_AFTER_THE_PARTY_HAT});
}

/**
 * Given a wallet
 * When ACE buys a look of a name no look has
 * Then the master answers 404
 */
TEST_F(WalletRoutesFixture, RefusesALookNoneHas) {
  const httplib::Result refused =
      send(ownCall(PUT_METHOD, UNKNOWN_LOOK_URL, token()));

  ASSERT_TRUE(refused);
  EXPECT_EQ(refused->status, EXPECTED_NOT_FOUND);
}

/**
 * Given a wallet of 1000 kopecks
 * When ACE buys the golden Bydo, at 10000 kopecks
 * Then the master answers 409
 */
TEST_F(WalletRoutesFixture, RefusesALookPastTheKopecks) {
  const httplib::Result refused =
      send(ownCall(PUT_METHOD, GOLDEN_BYDO_URL, token()));

  ASSERT_TRUE(refused);
  EXPECT_EQ(refused->status, EXPECTED_CONFLICT);
}

/**
 * Given a wallet that just opened
 * When ACE lists its looks
 * Then the master answers 200 with the three looks of the starter kit
 */
TEST_F(WalletRoutesFixture, ListsTheLooksOfTheStarterKit) {
  const httplib::Result looks =
      send(ownCall(GET_METHOD_TEXT, OWN_LOOKS_URL, token()));

  ASSERT_TRUE(looks);
  EXPECT_EQ(looks->status, EXPECTED_OK);
  EXPECT_EQ(arraySizeIn(looks, ITEMS_MEMBER_NAME),
            std::optional{STARTER_LOOK_COUNT});
}

/**
 * Given a wallet that opened with kopecks and chips, then made an exchange
 * When ACE reads one line of its journal, then a page of none
 * Then the master answers one line, then 400
 */
TEST_F(WalletRoutesFixture, PaginatesTheJournal) {
  ASSERT_TRUE(
      send(exchangeCall(token(), std::string{TWO_HUNDRED_KOPECKS_BODY})));
  const std::string onePage =
      std::string{OWN_MOVEMENTS_URL} + std::string{ONE_LINE_QUERY};
  const std::string noPage =
      std::string{OWN_MOVEMENTS_URL} + std::string{NO_LINE_QUERY};

  const httplib::Result page = send(ownCall(GET_METHOD_TEXT, onePage, token()));
  const httplib::Result refused =
      send(ownCall(GET_METHOD_TEXT, noPage, token()));

  EXPECT_EQ(arraySizeIn(page, MOVEMENTS_MEMBER_NAME),
            std::optional{LINES_OF_ONE_PAGE});
  ASSERT_TRUE(refused);
  EXPECT_EQ(refused->status, EXPECTED_BAD_REQUEST);
}

/**
 * Given a master whose master.cfg turns its wallets off
 * When it starts and ACE creates an account, then reads a wallet
 * Then the accounts answer, and no route of the wallets does
 */
TEST(WalletRoutes, AMasterWithItsWalletsOffStarts) {
  const RunningMaster master(walletOffLines());

  const httplib::Result created = sendRouteCall(
      master.port(), credentialsCall(USERS_URL, aceCredentials()));
  const httplib::Result refused = sendRouteCall(
      master.port(), ownCall(GET_METHOD_TEXT, OWN_WALLET_URL, {}));

  ASSERT_TRUE(created);
  ASSERT_TRUE(refused);
  EXPECT_EQ(created->status, EXPECTED_CREATED);
  EXPECT_EQ(refused->status, EXPECTED_NOT_FOUND);
}
