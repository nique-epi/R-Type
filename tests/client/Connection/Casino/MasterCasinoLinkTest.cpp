#include <gtest/gtest.h>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "CannedAnswer.hpp"
#include "CasinoReadings.hpp"
#include "CasinoRefusal.hpp"
#include "FakeAccountMasterConstants.hpp"
#include "MasterCasinoLinkFixture.hpp"
#include "MasterCasinoLinkTestConstants.hpp"
#include "MasterMessageText.hpp"
#include "RecordedRequest.hpp"

using rtype::client::CasinoReadings;
using rtype::client::CasinoRefusal;
using rtype::client::testing::BETS_CLOSED_ANSWER;
using rtype::client::testing::CASINO_CONFLICT_STATUS;
using rtype::client::testing::CASINO_LINK_TOKEN;
using rtype::client::testing::CASINO_NO_CONTENT_STATUS;
using rtype::client::testing::CASINO_OK_STATUS;
using rtype::client::testing::CASINO_UNPROCESSABLE_STATUS;
using rtype::client::testing::LINK_BETS_ROUTE;
using rtype::client::testing::LINK_LEAVE_ROUTE;
using rtype::client::testing::LINK_ROOM;
using rtype::client::testing::LINK_ROOM_ROUTE;
using rtype::client::testing::LINK_ROUND;
using rtype::client::testing::LINK_SEAT_ROUTE;
using rtype::client::testing::MasterCasinoLinkFixture;
using rtype::client::testing::NOT_ENOUGH_CHIPS_ANSWER;
using rtype::client::testing::NOT_SEATED_ANSWER;
using rtype::client::testing::ROOM_FULL_ANSWER;
using rtype::client::testing::SEATED_IN_LINK_ROOM;
using rtype::client::testing::UP_FROM_LINK_ROOM;
using rtype::network::casinoRoomStateText;

/**
 * Given a master that answers the state of the room of the tests
 * When the link asks that room
 * Then its reading holds the state, for the room asked
 */
TEST_F(MasterCasinoLinkFixture, ReadsTheStateOfTheRoomAsked) {
  master().answer(std::string{LINK_ROOM_ROUTE},
                  CannedAnswer{.status = CASINO_OK_STATUS,
                               .body = casinoRoomStateText(linkRoomState())});

  const std::uint64_t ask = link().askRoom(LINK_ROOM);
  const std::optional<CasinoReadings> readings =
      readingsSoon([ask](const CasinoReadings& read) {
        return read.room.has_value() && read.room->ask == ask;
      });

  ASSERT_TRUE(readings.has_value());
  EXPECT_EQ(readings->room->state, linkRoomState());
}

/**
 * Given a master with a seat free in the room of the tests
 * When the link asks a seat, then bets
 * Then the seat is taken before the bets go
 */
TEST_F(MasterCasinoLinkFixture, TakesTheSeatBeforeLayingTheBets) {
  master().answer(std::string{LINK_SEAT_ROUTE},
                  CannedAnswer{.status = CASINO_OK_STATUS,
                               .body = casinoRoomStateText(linkRoomState())});

  static_cast<void>(
      link().askSeat(std::string{CASINO_LINK_TOKEN}, SEATED_IN_LINK_ROOM));
  const std::uint64_t ask =
      link().askBets(std::string{CASINO_LINK_TOKEN}, LINK_ROUND, tenOnRed());
  ASSERT_TRUE(readingsSoon([ask](const CasinoReadings& read) {
                return read.bets.has_value() && read.bets->ask == ask;
              }).has_value());

  const std::vector<RecordedRequest> requests = master().requests();
  ASSERT_EQ(requests.size(), 2U);
  EXPECT_EQ(requests.front().method + std::string{ROUTE_METHOD_SEPARATOR} +
                requests.front().path,
            std::string{LINK_SEAT_ROUTE});
}

/**
 * Given a seat in the room of the tests
 * When the link asks the player up
 * Then the master is asked to delete the seat
 */
TEST_F(MasterCasinoLinkFixture, GetsThePlayerUp) {
  master().answer(std::string{LINK_LEAVE_ROUTE},
                  CannedAnswer{.status = CASINO_NO_CONTENT_STATUS, .body = {}});

  const std::uint64_t ask =
      link().askSeat(std::string{CASINO_LINK_TOKEN}, UP_FROM_LINK_ROOM);
  const std::optional<CasinoReadings> readings =
      readingsSoon([ask](const CasinoReadings& read) {
        return read.seat.has_value() && read.seat->ask == ask;
      });

  ASSERT_TRUE(readings.has_value());
  EXPECT_EQ(readings->seat->refusal, std::nullopt);
}

/**
 * Given a room whose every seat is taken
 * When the link asks a seat in it
 * Then its reading says the room is full
 */
TEST_F(MasterCasinoLinkFixture, SaysAFullRoomRefusedTheSeat) {
  master().answer(std::string{LINK_SEAT_ROUTE},
                  CannedAnswer{.status = CASINO_CONFLICT_STATUS,
                               .body = std::string{ROOM_FULL_ANSWER}});

  const std::uint64_t ask =
      link().askSeat(std::string{CASINO_LINK_TOKEN}, SEATED_IN_LINK_ROOM);
  const std::optional<CasinoReadings> readings =
      readingsSoon([ask](const CasinoReadings& read) {
        return read.seat.has_value() && read.seat->ask == ask;
      });

  ASSERT_TRUE(readings.has_value());
  EXPECT_EQ(readings->seat->refusal, CasinoRefusal::RoomFull);
}

/**
 * Given a master whose round of the tests closed its bets
 * When the link lays bets on it
 * Then its reading says the bets closed
 */
TEST_F(MasterCasinoLinkFixture, SaysTheBetsClosed) {
  master().answer(std::string{LINK_BETS_ROUTE},
                  CannedAnswer{.status = CASINO_CONFLICT_STATUS,
                               .body = std::string{BETS_CLOSED_ANSWER}});

  const std::uint64_t ask =
      link().askBets(std::string{CASINO_LINK_TOKEN}, LINK_ROUND, tenOnRed());
  const std::optional<CasinoReadings> readings =
      readingsSoon([ask](const CasinoReadings& read) {
        return read.bets.has_value() && read.bets->ask == ask;
      });

  ASSERT_TRUE(readings.has_value());
  EXPECT_EQ(readings->bets->refusal, CasinoRefusal::BetsClosed);
}

/**
 * Given a master that answers the same status to a player with no seat
 * When the link lays bets for such a player
 * Then its reading says the player sits in no seat, not that the bets closed
 */
TEST_F(MasterCasinoLinkFixture, TellsANoSeatFromClosedBets) {
  master().answer(std::string{LINK_BETS_ROUTE},
                  CannedAnswer{.status = CASINO_CONFLICT_STATUS,
                               .body = std::string{NOT_SEATED_ANSWER}});

  const std::uint64_t ask =
      link().askBets(std::string{CASINO_LINK_TOKEN}, LINK_ROUND, tenOnRed());
  const std::optional<CasinoReadings> readings =
      readingsSoon([ask](const CasinoReadings& read) {
        return read.bets.has_value() && read.bets->ask == ask;
      });

  ASSERT_TRUE(readings.has_value());
  EXPECT_EQ(readings->bets->refusal, CasinoRefusal::NotSeated);
}

/**
 * Given a master whose wallet holds fewer chips than the bets
 * When the link lays them
 * Then its reading says the chips are too few, not that a rule was broken
 */
TEST_F(MasterCasinoLinkFixture, TellsTooFewChipsFromABrokenRule) {
  master().answer(std::string{LINK_BETS_ROUTE},
                  CannedAnswer{.status = CASINO_UNPROCESSABLE_STATUS,
                               .body = std::string{NOT_ENOUGH_CHIPS_ANSWER}});

  const std::uint64_t ask =
      link().askBets(std::string{CASINO_LINK_TOKEN}, LINK_ROUND, tenOnRed());
  const std::optional<CasinoReadings> readings =
      readingsSoon([ask](const CasinoReadings& read) {
        return read.bets.has_value() && read.bets->ask == ask;
      });

  ASSERT_TRUE(readings.has_value());
  EXPECT_EQ(readings->bets->refusal, CasinoRefusal::NotEnoughChips);
}
