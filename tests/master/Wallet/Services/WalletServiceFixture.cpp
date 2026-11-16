#include "WalletServiceFixture.hpp"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <cstdint>
#include <string>
#include <string_view>
#include "ChipMovement.hpp"
#include "ChipMovementResult.hpp"
#include "Currency.hpp"
#include "GameCredit.hpp"
#include "MasterDatabase.hpp"
#include "MasterMessageText.hpp"
#include "MovementReason.hpp"
#include "Wallet.hpp"
#include "WalletService.hpp"
#include "WalletServiceTestConstants.hpp"

namespace {

std::int64_t insertedAce(rtype::master::MasterDatabase& database) {
  return database.run([](SQLite::Database& connection) {
    SQLite::Statement insert{connection,
                             std::string{INSERT_TEST_USER_STATEMENT}};
    insert.bind(std::string{TEST_USER_NICKNAME_PARAMETER},
                std::string{WALLET_TEST_NICKNAME});
    insert.executeStep();
    return insert.getColumn(FIRST_RESULT_COLUMN).getInt64();
  });
}

}  // namespace

WalletServiceFixture::WalletServiceFixture()
    : ace_(insertedAce(file_.database())), wallets_(file_.database(), [] {
        return rtype::network::Wallet{TEST_STARTING_WALLET};
      }) {}

rtype::master::WalletService& WalletServiceFixture::wallets() {
  return wallets_;
}

rtype::master::MasterDatabase& WalletServiceFixture::database() {
  return file_.database();
}

std::int64_t WalletServiceFixture::ace() const { return ace_; }

std::int64_t WalletServiceFixture::journalSumOf(
    rtype::network::Currency currency) {
  return file_.database().run([this, currency](SQLite::Database& connection) {
    SQLite::Statement query{connection, std::string{JOURNAL_SUM_QUERY}};
    query.bind(std::string{JOURNAL_USER_PARAMETER}, ace_);
    query.bind(std::string{JOURNAL_CURRENCY_PARAMETER},
               std::string{rtype::network::currencyText(currency)});
    query.executeStep();
    return query.getColumn(FIRST_RESULT_COLUMN).getInt64();
  });
}

void WalletServiceFixture::forgetLook(std::string_view item) {
  file_.database().run([this, item](SQLite::Database& connection) {
    SQLite::Statement forget{connection, std::string{FORGET_LOOK_STATEMENT}};
    forget.bind(std::string{JOURNAL_USER_PARAMETER}, ace_);
    forget.bind(std::string{FORGET_LOOK_ITEM_PARAMETER}, std::string{item});
    forget.exec();
  });
}

rtype::master::ChipMovement WalletServiceFixture::stakeOf(
    std::int64_t amount) const {
  return rtype::master::ChipMovement{
      .userId = ace_,
      .amount = amount,
      .reason = rtype::network::MovementReason::Stake,
      .reference = std::string{STAKE_REFERENCE},
      .appliedOnce = false,
      .madeAt = WALLET_TEST_DATE};
}

rtype::master::ChipMovement WalletServiceFixture::payoutOf(
    std::int64_t amount) const {
  return rtype::master::ChipMovement{
      .userId = ace_,
      .amount = amount,
      .reason = rtype::network::MovementReason::Payout,
      .reference = std::string{PAYOUT_REFERENCE},
      .appliedOnce = true,
      .madeAt = WALLET_TEST_DATE};
}

rtype::master::ChipMovementResult WalletServiceFixture::applied(
    const rtype::master::ChipMovement& movement) {
  return file_.database().transact(
      [this, &movement](SQLite::Database& connection) {
        return wallets_.applyChips(connection, movement);
      });
}

rtype::master::GameCredit WalletServiceFixture::gameCreditOf(
    std::int64_t userId) {
  return rtype::master::GameCredit{
      .userId = userId,
      .kopecks = GAME_KOPECKS,
      .wonLooks = {std::string{WON_LOOK_NAME}, std::string{NO_LOOK_NAME}},
      .reference = std::string{GAME_REFERENCE},
      .creditedAt = WALLET_TEST_DATE};
}
