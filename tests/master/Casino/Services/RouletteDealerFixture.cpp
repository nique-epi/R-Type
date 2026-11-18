#include "RouletteDealerFixture.hpp"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <chrono>
#include <cstdint>
#include <string>
#include <variant>
#include <vector>
#include "BetOrder.hpp"
#include "BetRefusal.hpp"
#include "BetsReceipt.hpp"
#include "CasinoService.hpp"
#include "DrawnRound.hpp"
#include "MasterDatabase.hpp"
#include "MillisecondDate.hpp"
#include "RoomRound.hpp"
#include "RouletteBet.hpp"
#include "RouletteBetKind.hpp"
#include "RouletteDealerTestConstants.hpp"
#include "RouletteSchedule.hpp"
#include "ScriptedRouletteDraw.hpp"
#include "SeatKey.hpp"
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

RouletteDealerFixture::RouletteDealerFixture()
    : ace_(insertedAce(file_.database())),
      wallets_(file_.database(),
               [] { return rtype::network::Wallet{TEST_STARTING_WALLET}; }),
      draw_({RED_NINETEEN, ZERO}),
      casino_(file_.database(), wallets_, [] { return DEALER_TEST_SETTINGS; }),
      dealer_(file_.database(), wallets_, draw_) {}

rtype::master::CasinoService& RouletteDealerFixture::casino() {
  return casino_;
}

std::int64_t RouletteDealerFixture::ace() const { return ace_; }

rtype::network::testing::ScriptedRouletteDraw& RouletteDealerFixture::draw() {
  return draw_;
}

rtype::master::RouletteDealer& RouletteDealerFixture::dealer() {
  return dealer_;
}

std::variant<rtype::network::BetsReceipt, rtype::master::BetRefusal>
RouletteDealerFixture::aceBetsAt(rtype::network::MillisecondDate sentAt) {
  static_cast<void>(
      casino_.takeSeat(rtype::master::SeatKey{.room = BET_ROOM, .userId = ace_},
                       std::chrono::floor<std::chrono::seconds>(sentAt)));
  return casino_.placeBets(rtype::master::BetOrder{
      .place =
          rtype::network::RoomRound{
              .room = BET_ROOM,
              .round = rtype::master::scheduledRoundAt(
                           sentAt, DEALER_TEST_SETTINGS.durations)
                           .round},
      .userId = ace_,
      .bets = {rtype::network::RouletteBet{
                   .kind = rtype::network::RouletteBetKind::Straight,
                   .firstNumber = STRAIGHT_ON,
                   .amount = STRAIGHT_STAKE},
               rtype::network::RouletteBet{
                   .kind = rtype::network::RouletteBetKind::Red,
                   .firstNumber = 0,
                   .amount = RED_STAKE}},
      .sentAt = sentAt});
}

std::vector<rtype::master::DrawnRound> RouletteDealerFixture::drawAt(
    rtype::network::MillisecondDate now) {
  return dealer_.drawDueRounds(now, DEALER_TEST_SETTINGS);
}

std::vector<rtype::master::DrawnRound> RouletteDealerFixture::drawMissedAt(
    rtype::network::MillisecondDate now) {
  return dealer_.drawMissedRounds(now, DEALER_TEST_SETTINGS);
}

std::uint32_t RouletteDealerFixture::chipsOfAce() {
  return wallets_.walletOf(ace_, WALLET_TEST_DATE).chips;
}
