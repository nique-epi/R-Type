#include <SQLiteCpp/Database.h>
#include <gtest/gtest.h>
#include <algorithm>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>
#include "ChipMovementResult.hpp"
#include "ChipRefusal.hpp"
#include "Currency.hpp"
#include "ExchangeReceipt.hpp"
#include "ExchangeRequest.hpp"
#include "GameCreditOutcome.hpp"
#include "LookPurchaseOutcome.hpp"
#include "MovementListing.hpp"
#include "MovementReason.hpp"
#include "OwnedLooks.hpp"
#include "PageRequest.hpp"
#include "Wallet.hpp"
#include "WalletExchange.hpp"
#include "WalletExchangeRefusal.hpp"
#include "WalletServiceFixture.hpp"
#include "WalletServiceTestConstants.hpp"

using rtype::master::ChipMovementResult;
using rtype::master::ChipRefusal;
using rtype::master::GameCreditOutcome;
using rtype::master::LookPurchaseOutcome;
using rtype::master::PageRequest;
using rtype::network::Currency;
using rtype::network::ExchangeReceipt;
using rtype::network::ExchangeRequest;
using rtype::network::MovementReason;
using rtype::network::Wallet;
using rtype::network::WalletExchangeRefusal;

/**
 * Given the account ACE, whose wallet was never read
 * When its wallet and its looks are read
 * Then the wallet opened with the starting balances and the starter kit
 */
TEST_F(WalletServiceFixture, AWalletOpensWithTheStartingBalancesAndLooks) {
  EXPECT_EQ(wallets().walletOf(ace(), WALLET_TEST_DATE), TEST_STARTING_WALLET);
  EXPECT_EQ(wallets().ownedLooksOf(ace(), WALLET_TEST_DATE).items,
            (std::vector<std::string>{std::string{STARTER_TOP_HAT_NAME},
                                      std::string{STARTER_COOL_SHADES_NAME},
                                      std::string{STARTER_MUSTACHE_NAME}}));
}

/**
 * Given a wallet of 1000 kopecks and 500 chips
 * When 200 kopecks change into chips, then 200 chips into kopecks
 * Then each exchange moves the amount one for one, and the journal of each
 * currency sums to its balance
 */
TEST_F(WalletServiceFixture, AnExchangeMovesTheAmountOneForOne) {
  const auto intoChips = wallets().exchange(
      ace(), ExchangeRequest{.from = Currency::Kopecks, .amount = TWO_HUNDRED},
      WALLET_TEST_DATE);
  ASSERT_TRUE(std::holds_alternative<ExchangeReceipt>(intoChips));
  EXPECT_EQ(std::get<ExchangeReceipt>(intoChips).wallet,
            (Wallet{.kopecks = 800, .chips = 700}));

  const auto intoKopecks = wallets().exchange(
      ace(), ExchangeRequest{.from = Currency::Chips, .amount = TWO_HUNDRED},
      WALLET_TEST_DATE);

  ASSERT_TRUE(std::holds_alternative<ExchangeReceipt>(intoKopecks));
  EXPECT_EQ(wallets().walletOf(ace(), WALLET_TEST_DATE), TEST_STARTING_WALLET);
  EXPECT_EQ(journalSumOf(Currency::Kopecks), TEST_STARTING_WALLET.kopecks);
  EXPECT_EQ(journalSumOf(Currency::Chips), TEST_STARTING_WALLET.chips);
}

/**
 * Given a wallet of 1000 kopecks
 * When 1001 kopecks change into chips
 * Then the cashier refuses it, and nothing moved
 */
TEST_F(WalletServiceFixture, AnExchangeOfMoreThanTheBalanceIsRefused) {
  const auto refused = wallets().exchange(
      ace(),
      ExchangeRequest{.from = Currency::Kopecks,
                      .amount = MORE_THAN_THE_STARTING_KOPECKS},
      WALLET_TEST_DATE);

  ASSERT_TRUE(std::holds_alternative<WalletExchangeRefusal>(refused));
  EXPECT_EQ(std::get<WalletExchangeRefusal>(refused),
            WalletExchangeRefusal::NotEnough);
  EXPECT_EQ(wallets().walletOf(ace(), WALLET_TEST_DATE), TEST_STARTING_WALLET);
}

/**
 * Given a wallet
 * When an exchange of no amount, then one past the largest balance, is asked
 * Then the cashier refuses both as invalid amounts
 */
TEST_F(WalletServiceFixture, AnExchangeOfNoneOrPastTheLargestIsInvalid) {
  for (const std::uint32_t amount : {NO_AMOUNT, PAST_THE_LARGEST_BALANCE}) {
    const auto refused = wallets().exchange(
        ace(), ExchangeRequest{.from = Currency::Chips, .amount = amount},
        WALLET_TEST_DATE);
    ASSERT_TRUE(std::holds_alternative<WalletExchangeRefusal>(refused));
    EXPECT_EQ(std::get<WalletExchangeRefusal>(refused),
              WalletExchangeRefusal::InvalidAmount);
  }
}

/**
 * Given an exchange ACE made
 * When it is read back by its number
 * Then it is the same exchange
 */
TEST_F(WalletServiceFixture, AnExchangeIsReadBackByItsNumber) {
  const auto made = wallets().exchange(
      ace(), ExchangeRequest{.from = Currency::Kopecks, .amount = TWO_HUNDRED},
      WALLET_TEST_DATE);
  ASSERT_TRUE(std::holds_alternative<ExchangeReceipt>(made));
  const rtype::network::WalletExchange exchange =
      std::get<ExchangeReceipt>(made).exchange;

  EXPECT_EQ(wallets().exchangeOf(ace(), exchange.id), std::optional{exchange});
}

/**
 * Given a wallet of 1000 kopecks
 * When ACE buys the party hat twice
 * Then the first buys it for 150 kopecks, and the second takes nothing
 */
TEST_F(WalletServiceFixture, BuyingALookTwiceTakesItsPriceOnce) {
  const auto first = wallets().buyLook(ace(), PARTY_HAT_ITEM, WALLET_TEST_DATE);
  const auto second =
      wallets().buyLook(ace(), PARTY_HAT_ITEM, WALLET_TEST_DATE);

  EXPECT_EQ(first.outcome, LookPurchaseOutcome::Bought);
  EXPECT_EQ(second.outcome, LookPurchaseOutcome::AlreadyOwned);
  EXPECT_EQ(wallets().walletOf(ace(), WALLET_TEST_DATE).kopecks,
            TEST_STARTING_WALLET.kopecks - PARTY_HAT_PRICE_PAID);
}

/**
 * Given a wallet of 1000 kopecks that no longer owns the top hat
 * When ACE buys the top hat, which the shop does not sell
 * Then the shop refuses it, and nothing moved
 */
TEST_F(WalletServiceFixture, TheShopRefusesALookItDoesNotSell) {
  static_cast<void>(wallets().walletOf(ace(), WALLET_TEST_DATE));
  forgetLook(STARTER_TOP_HAT_NAME);

  const auto refused = wallets().buyLook(ace(), TOP_HAT_ITEM, WALLET_TEST_DATE);

  EXPECT_EQ(refused.outcome, LookPurchaseOutcome::NotForSale);
  EXPECT_EQ(wallets().walletOf(ace(), WALLET_TEST_DATE), TEST_STARTING_WALLET);
}

/**
 * Given a wallet of 1000 kopecks
 * When ACE buys the golden Bydo, at 10000 kopecks
 * Then the shop refuses it as too expensive, and nothing moved
 */
TEST_F(WalletServiceFixture, TheShopRefusesALookPastTheKopecks) {
  const auto refused =
      wallets().buyLook(ace(), GOLDEN_BYDO_ITEM, WALLET_TEST_DATE);

  EXPECT_EQ(refused.outcome, LookPurchaseOutcome::NotEnoughKopecks);
  EXPECT_EQ(wallets().walletOf(ace(), WALLET_TEST_DATE), TEST_STARTING_WALLET);
}

/**
 * Given a wallet of 500 chips
 * When the casino stakes 100 chips in a transaction it then rolls back
 * Then the stake applied inside it, and nothing stays once it rolled back
 */
TEST_F(WalletServiceFixture, ChipsMoveInsideTheTransactionOfTheCaller) {
  static_cast<void>(wallets().walletOf(ace(), WALLET_TEST_DATE));
  std::optional<ChipMovementResult> inside;

  EXPECT_THROW(
      database().transact([this, &inside](SQLite::Database& connection) {
        inside =
            wallets().applyChips(connection, stakeOf(A_STAKE_OF_A_HUNDRED));
        throw std::runtime_error{std::string{ROLLED_BACK_TEXT}};
      }),
      std::runtime_error);

  ASSERT_TRUE(inside.has_value());
  EXPECT_EQ(inside.value_or(ChipMovementResult{}).chips,
            TEST_STARTING_WALLET.chips + A_STAKE_OF_A_HUNDRED);
  EXPECT_EQ(wallets().walletOf(ace(), WALLET_TEST_DATE), TEST_STARTING_WALLET);
}

/**
 * Given a wallet of 500 chips
 * When the casino stakes 501 chips
 * Then the stake is refused, and the chips stay
 */
TEST_F(WalletServiceFixture, AStakePastTheChipsIsRefused) {
  const ChipMovementResult refused = applied(stakeOf(A_STAKE_PAST_THE_CHIPS));

  EXPECT_EQ(refused.refusal, std::optional{ChipRefusal::NotEnoughChips});
  EXPECT_EQ(refused.chips, TEST_STARTING_WALLET.chips);
}

/**
 * Given a game that pays ACE 1245 kopecks and the crown
 * When it is credited twice
 * Then the second credits nothing: the kopecks and the crown came once
 */
TEST_F(WalletServiceFixture, AGameIsCreditedOnce) {
  const GameCreditOutcome first = wallets().creditGame(gameCreditOf(ace()));
  const GameCreditOutcome second = wallets().creditGame(gameCreditOf(ace()));

  EXPECT_EQ(first, GameCreditOutcome::Credited);
  EXPECT_EQ(second, GameCreditOutcome::AlreadyCredited);
  EXPECT_EQ(wallets().walletOf(ace(), WALLET_TEST_DATE).kopecks,
            TEST_STARTING_WALLET.kopecks + GAME_KOPECKS);
  const std::vector<std::string> looks =
      wallets().ownedLooksOf(ace(), WALLET_TEST_DATE).items;
  EXPECT_EQ(std::ranges::count(looks, std::string{WON_LOOK_NAME}), 1);
}

/**
 * Given a game credited to an id no account has
 * When it is credited
 * Then nothing is credited, and the outcome says the account is unknown
 */
TEST_F(WalletServiceFixture, AGameOfAnUnknownAccountCreditsNothing) {
  EXPECT_EQ(wallets().creditGame(gameCreditOf(UNKNOWN_USER_ID)),
            GameCreditOutcome::UnknownAccount);
}

/**
 * Given a wallet that opened, then changed 200 kopecks into chips
 * When the first page of one line of its journal is read
 * Then it holds the newest line: the chips the exchange brought
 */
TEST_F(WalletServiceFixture, TheJournalIsReadNewestFirst) {
  ASSERT_TRUE(std::holds_alternative<ExchangeReceipt>(wallets().exchange(
      ace(), ExchangeRequest{.from = Currency::Kopecks, .amount = TWO_HUNDRED},
      WALLET_TEST_DATE)));

  const rtype::network::MovementListing page = wallets().movementsOf(
      ace(), PageRequest{.limit = ONE_LINE, .offset = FROM_THE_FIRST},
      WALLET_TEST_DATE);

  ASSERT_EQ(page.movements.size(), 1U);
  EXPECT_EQ(page.movements.front().currency, Currency::Chips);
  EXPECT_EQ(page.movements.front().amount, TWO_HUNDRED);
  EXPECT_EQ(page.movements.front().reason, MovementReason::Exchange);
}
