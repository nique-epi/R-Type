#include <gtest/gtest.h>
#include <string>
#include "CannedAnswer.hpp"
#include "CasinoRefusalConstants.hpp"
#include "FakeAccountMasterConstants.hpp"
#include "MasterAccountClientTestConstants.hpp"
#include "MasterCasinoClientFixture.hpp"
#include "MasterCasinoClientTestConstants.hpp"
#include "MasterMessageText.hpp"
#include "NetworkException.hpp"

using rtype::network::BETS_CLOSED_DETAIL;
using rtype::network::betsReceiptText;
using rtype::network::casinoRoomListingText;
using rtype::network::casinoRoomStateText;
using rtype::network::MasterRefusedException;
using rtype::network::rouletteBetsText;
using rtype::network::roundBetsText;

/**
 * Given a master running one room
 * When the client lists the rooms
 * Then it reads that room
 */
TEST_F(MasterCasinoClientFixture, ListsTheRoomsOfTheCasino) {
  master().answer(std::string{LIST_CASINO_ROOMS},
                  CannedAnswer{.status = OK_STATUS,
                               .body = casinoRoomListingText(testRooms())});

  EXPECT_EQ(client().listRooms(), testRooms());
}

/**
 * Given a master that answers the state of the room of the tests
 * When the client reads that room
 * Then it reads its state
 */
TEST_F(MasterCasinoClientFixture, ReadsTheStateOfOneRoom) {
  master().answer(std::string{READ_TEST_ROOM},
                  CannedAnswer{.status = OK_STATUS,
                               .body = casinoRoomStateText(testRoomState())});

  EXPECT_EQ(client().readRoom(CASINO_TEST_ROOM), testRoomState());
}

/**
 * Given a master with a free seat in the room of the tests
 * When the client takes a seat there
 * Then the token goes as Bearer
 */
TEST_F(MasterCasinoClientFixture, TakesASeatWithTheTokenAsBearer) {
  master().answer(std::string{TAKE_TEST_SEAT},
                  CannedAnswer{.status = CREATED_STATUS,
                               .body = casinoRoomStateText(testRoomState())});

  static_cast<void>(
      client().takeSeat(std::string{ACCOUNT_TOKEN}, CASINO_TEST_ROOM));

  EXPECT_EQ(lastRequest().authorization, BEARER_ACCOUNT_TOKEN);
}

/**
 * Given a master that answers a seat left with no content
 * When the client leaves its seat in the room of the tests
 * Then the master is asked to delete the seat of the token
 */
TEST_F(MasterCasinoClientFixture, LeavesTheSeatOfTheToken) {
  master().answer(std::string{LEAVE_TEST_SEAT},
                  CannedAnswer{.status = SEAT_LEFT_STATUS, .body = {}});

  client().leaveSeat(std::string{ACCOUNT_TOKEN}, CASINO_TEST_ROOM);

  EXPECT_EQ(lastRequest().method + std::string{ROUTE_METHOD_SEPARATOR} +
                lastRequest().path,
            std::string{LEAVE_TEST_SEAT});
}

/**
 * Given a master that takes bets on the round of the tests
 * When the client places ten chips on 17
 * Then the body of the request holds them as the master reads bets
 */
TEST_F(MasterCasinoClientFixture, SendsTheBetsAsTheMasterReadsThem) {
  master().answer(std::string{PLACE_TEST_BETS},
                  CannedAnswer{.status = OK_STATUS,
                               .body = betsReceiptText(tenOn17Receipt())});

  static_cast<void>(client().placeBets(std::string{ACCOUNT_TOKEN},
                                       CASINO_TEST_ROUND, tenOn17()));

  EXPECT_EQ(lastRequest().body, rouletteBetsText(tenOn17()));
}

/**
 * Given a master that takes bets on the round of the tests
 * When the client places ten chips on 17
 * Then it reads the receipt, the chips left included
 */
TEST_F(MasterCasinoClientFixture, ReadsTheReceiptOfTheBets) {
  master().answer(std::string{PLACE_TEST_BETS},
                  CannedAnswer{.status = OK_STATUS,
                               .body = betsReceiptText(tenOn17Receipt())});

  EXPECT_EQ(client().placeBets(std::string{ACCOUNT_TOKEN}, CASINO_TEST_ROUND,
                               tenOn17()),
            tenOn17Receipt());
}

/**
 * Given a master whose round of the tests drew 17
 * When the client reads its own bets on that round
 * Then it reads what they paid
 */
TEST_F(MasterCasinoClientFixture, ReadsWhatTheOwnBetsPaid) {
  master().answer(
      std::string{READ_TEST_BETS},
      CannedAnswer{.status = OK_STATUS, .body = roundBetsText(tenOn17Paid())});

  EXPECT_EQ(client().readOwnBets(std::string{ACCOUNT_TOKEN}, CASINO_TEST_ROUND),
            tenOn17Paid());
}

/**
 * Given a master that refuses bets on a closed round
 * When the client places bets on it
 * Then the refusal it throws keeps the detail of the master
 */
TEST_F(MasterCasinoClientFixture, ARefusalKeepsTheDetailOfTheMaster) {
  master().answer(std::string{PLACE_TEST_BETS},
                  CannedAnswer{.status = CONFLICT_STATUS,
                               .body = std::string{BETS_CLOSED_PROBLEM}});

  try {
    static_cast<void>(client().placeBets(std::string{ACCOUNT_TOKEN},
                                         CASINO_TEST_ROUND, tenOn17()));
    FAIL() << "the bets were not refused";
  } catch (const MasterRefusedException& refused) {
    EXPECT_EQ(refused.detail(), BETS_CLOSED_DETAIL);
  }
}
