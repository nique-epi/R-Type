#include <gtest/gtest.h>
#include <httplib.h>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <vector>
#include "AccountsRoutesFixtures.hpp"
#include "AccountsRoutesTestConstants.hpp"
#include "CasinoRoutesFixture.hpp"
#include "CasinoRoutesFixtures.hpp"
#include "CasinoRoutesTestConstants.hpp"
#include "LeaderboardRoutesFixtures.hpp"
#include "MasterTestConstants.hpp"
#include "RouteCall.hpp"
#include "RouteCallFixtures.hpp"
#include "RunningMaster.hpp"
#include "ServersRoutesTestConstants.hpp"
#include "WalletRoutesFixtures.hpp"

/**
 * Given a casino of two rooms
 * When a client lists the rooms in plain HTTP, without a token
 * Then the master answers 200 with both rooms, the first named Bydo Palace
 */
TEST_F(CasinoRoutesFixture, ListsTheRoomsToEveryone) {
  RouteCall call = ownCall(GET_METHOD_TEXT, ROOMS_URL, {});
  call.overHttps = false;

  const httplib::Result rooms = send(call);

  ASSERT_TRUE(rooms);
  EXPECT_EQ(rooms->status, EXPECTED_OK);
  const nlohmann::json listing = bodyOf(rooms);
  ASSERT_EQ(listing.at(std::string{ROOMS_MEMBER_NAME}).size(), TEST_ROOM_COUNT);
  EXPECT_EQ(listing.at(std::string{ROOMS_MEMBER_NAME})
                .at(0)
                .at(std::string{NAME_MEMBER_NAME})
                .get<std::string>(),
            FIRST_ROOM_NAME);
}

/**
 * Given a room whose bets are open
 * When a client reads it in plain HTTP, without a token
 * Then the master answers 200, and no number shows before the bets close
 */
TEST_F(CasinoRoutesFixture, AnOpenRoomShowsNoNumber) {
  ASSERT_TRUE(openRound().has_value());
  RouteCall call = ownCall(GET_METHOD_TEXT, FIRST_ROOM_URL, {});
  call.overHttps = false;

  const httplib::Result room = send(call);

  ASSERT_TRUE(room);
  EXPECT_EQ(room->status, EXPECTED_OK);
  EXPECT_TRUE(
      bodyOf(room).at(std::string{WINNING_NUMBER_MEMBER_NAME}).is_null());
}

/**
 * Given a casino of two rooms
 * When a client reads the third, then ACE asks for a seat in it
 * Then the master answers 404 to both
 */
TEST_F(CasinoRoutesFixture, ARoomPastTheLastIsNotFound) {
  const httplib::Result room =
      send(ownCall(GET_METHOD_TEXT, NO_SUCH_ROOM_URL, {}));
  const httplib::Result seat =
      send(ownCall(PUT_METHOD, NO_SUCH_SEAT_URL, ace()));

  ASSERT_TRUE(room);
  ASSERT_TRUE(seat);
  EXPECT_EQ(room->status, EXPECTED_NOT_FOUND);
  EXPECT_EQ(seat->status, EXPECTED_NOT_FOUND);
}

/**
 * Given ACE without a seat
 * When ACE asks for a seat in the first room, then asks again
 * Then the master answers 201, then 200, and the room lists ACE
 */
TEST_F(CasinoRoutesFixture, ASeatIsTakenThenRenewed) {
  const httplib::Result taken = takeSeat(ace());
  const httplib::Result renewed = takeSeat(ace());

  ASSERT_TRUE(taken);
  ASSERT_TRUE(renewed);
  EXPECT_EQ(taken->status, EXPECTED_CREATED);
  EXPECT_EQ(renewed->status, EXPECTED_OK);
  EXPECT_EQ(bodyOf(renewed).at(std::string{PLAYERS_MEMBER_NAME}).size(), 1U);
}

/**
 * Given a room of one seat, which ACE holds
 * When R9 asks for a seat, then ACE leaves and R9 asks again
 * Then the master answers 409, then 204 to ACE, then 201 to R9
 */
TEST_F(CasinoRoutesFixture, AFullRoomSeatsNoOneMoreUntilASeatIsLeft) {
  ASSERT_EQ(takeSeat(ace())->status, EXPECTED_CREATED);

  const httplib::Result full = takeSeat(secondPilot());
  const httplib::Result left =
      send(ownCall(DELETE_METHOD_TEXT, FIRST_SEAT_URL, ace()));
  const httplib::Result seated = takeSeat(secondPilot());

  ASSERT_TRUE(full);
  ASSERT_TRUE(left);
  ASSERT_TRUE(seated);
  EXPECT_EQ(full->status, EXPECTED_CONFLICT);
  EXPECT_EQ(left->status, EXPECTED_NO_CONTENT);
  EXPECT_EQ(seated->status, EXPECTED_CREATED);
}

/**
 * Given ACE without a seat
 * When ACE leaves the first room
 * Then the master answers 204 all the same
 */
TEST_F(CasinoRoutesFixture, LeavingWithoutASeatSucceeds) {
  const httplib::Result left =
      send(ownCall(DELETE_METHOD_TEXT, FIRST_SEAT_URL, ace()));

  ASSERT_TRUE(left);
  EXPECT_EQ(left->status, EXPECTED_NO_CONTENT);
}

/**
 * Given a casino
 * When a seat is asked without a token, then by ACE in plain HTTP
 * Then the master answers 401, then 403
 */
TEST_F(CasinoRoutesFixture, ASeatNeedsASessionOverHttps) {
  RouteCall plain = ownCall(PUT_METHOD, FIRST_SEAT_URL, ace());
  plain.overHttps = false;

  const httplib::Result anonymous = takeSeat({});
  const httplib::Result inClear = send(plain);

  ASSERT_TRUE(anonymous);
  ASSERT_TRUE(inClear);
  EXPECT_EQ(anonymous->status, EXPECTED_UNAUTHORIZED);
  EXPECT_EQ(inClear->status, EXPECTED_FORBIDDEN);
}

/**
 * Given ACE seated, with 500 chips, and the bets of the round open
 * When ACE lays 10 on 19 and 25 on red, then replaces them by 100 on red
 * Then the first bets leave 465 chips, the second 400, and ACE reads 100
 * staked on the round, no number yet
 */
TEST_F(CasinoRoutesFixture, NewBetsReplaceTheOldOnes) {
  ASSERT_EQ(takeSeat(ace())->status, EXPECTED_CREATED);
  const std::optional<std::uint64_t> round = openRound();
  ASSERT_TRUE(round.has_value());
  const std::string path = ownBetsPathOf(round.value_or(0));

  const httplib::Result first = send(betsCall(path, ace(), TWO_BETS_BODY));
  const httplib::Result second =
      send(betsCall(path, ace(), HUNDRED_ON_RED_BODY));
  const httplib::Result read = send(ownCall(GET_METHOD_TEXT, path, ace()));

  ASSERT_TRUE(first);
  ASSERT_EQ(first->status, EXPECTED_OK);
  EXPECT_EQ(numberIn(first, STAKE_MEMBER_NAME), std::optional{TWO_BETS_STAKE});
  EXPECT_EQ(numberIn(first, CASINO_CHIPS_MEMBER_NAME),
            std::optional{CHIPS_AFTER_TWO_BETS});
  EXPECT_EQ(numberIn(second, CASINO_CHIPS_MEMBER_NAME),
            std::optional{CHIPS_AFTER_A_HUNDRED});
  EXPECT_EQ(numberIn(read, STAKE_MEMBER_NAME), std::optional{HUNDRED_STAKE});
  EXPECT_TRUE(
      bodyOf(read).at(std::string{WINNING_NUMBER_MEMBER_NAME}).is_null());
}

/**
 * Given ACE seated, with 35 chips on the round
 * When ACE sends an empty list of bets
 * Then every chip comes back: 500, none staked
 */
TEST_F(CasinoRoutesFixture, AnEmptyListTakesTheBetsBack) {
  ASSERT_EQ(takeSeat(ace())->status, EXPECTED_CREATED);
  const std::optional<std::uint64_t> round = openRound();
  ASSERT_TRUE(round.has_value());
  const std::string path = ownBetsPathOf(round.value_or(0));
  ASSERT_EQ(send(betsCall(path, ace(), TWO_BETS_BODY))->status, EXPECTED_OK);

  const httplib::Result back = send(betsCall(path, ace(), NO_BETS_BODY));

  ASSERT_TRUE(back);
  EXPECT_EQ(back->status, EXPECTED_OK);
  EXPECT_EQ(numberIn(back, CASINO_CHIPS_MEMBER_NAME),
            std::optional{OPENING_CHIPS});
  EXPECT_EQ(chipsOf(ace()), std::optional{OPENING_CHIPS});
}

/**
 * Given ACE seated, and the bets of the round open
 * When ACE bets on the round before
 * Then the master answers 409, and no chip moves
 */
TEST_F(CasinoRoutesFixture, BetsOnAnotherRoundAreClosed) {
  ASSERT_EQ(takeSeat(ace())->status, EXPECTED_CREATED);
  const std::optional<std::uint64_t> round = openRound();
  ASSERT_TRUE(round.has_value());
  const std::string path = ownBetsPathOf(round.value_or(0) - 1);

  const httplib::Result closed = send(betsCall(path, ace(), TWO_BETS_BODY));

  ASSERT_TRUE(closed);
  EXPECT_EQ(closed->status, EXPECTED_CONFLICT);
  EXPECT_EQ(chipsOf(ace()), std::optional{OPENING_CHIPS});
}

/**
 * Given ACE not seated, and the bets of the round open
 * When ACE bets
 * Then the master answers 409, and no chip moves
 */
TEST_F(CasinoRoutesFixture, BetsNeedASeat) {
  const std::optional<std::uint64_t> round = openRound();
  ASSERT_TRUE(round.has_value());

  const httplib::Result refused =
      send(betsCall(ownBetsPathOf(round.value_or(0)), ace(), TWO_BETS_BODY));

  ASSERT_TRUE(refused);
  EXPECT_EQ(refused->status, EXPECTED_CONFLICT);
  EXPECT_EQ(chipsOf(ace()), std::optional{OPENING_CHIPS});
}

/**
 * Given ACE seated, with 500 chips, and the bets of the round open
 * When ACE sends 33 bets, a split that crosses a street, a bet of no chip,
 * 2001 chips on red, or 501 chips on red
 * Then the master answers 422 to each, and no chip moves
 */
TEST_F(CasinoRoutesFixture, BetsTheRulesRefuseMoveNoChip) {
  ASSERT_EQ(takeSeat(ace())->status, EXPECTED_CREATED);
  const std::optional<std::uint64_t> round = openRound();
  ASSERT_TRUE(round.has_value());
  const std::string path = ownBetsPathOf(round.value_or(0));
  const std::vector<std::string> bodies{
      straightBetsOf(ONE_BET_MORE_THAN_A_ROUND_TAKES),
      std::string{CROSSING_SPLIT_BODY}, std::string{NO_CHIP_BODY},
      std::string{OVER_THE_CAP_BODY}, std::string{MORE_THAN_THE_WALLET_BODY}};

  for (const std::string& body : bodies) {
    const httplib::Result refused = send(betsCall(path, ace(), body));
    ASSERT_TRUE(refused);
    EXPECT_EQ(refused->status, EXPECTED_UNPROCESSABLE_CONTENT) << body;
  }

  EXPECT_EQ(chipsOf(ace()), std::optional{OPENING_CHIPS});
}

/**
 * Given ACE seated, with 35 chips on the round
 * When ACE replaces them by 501 chips, more than the 500 the refund leaves
 * Then the master answers 422, and the 35 chips stay on the cloth
 */
TEST_F(CasinoRoutesFixture, ARefusedStakeKeepsTheOldBets) {
  ASSERT_EQ(takeSeat(ace())->status, EXPECTED_CREATED);
  const std::optional<std::uint64_t> round = openRound();
  ASSERT_TRUE(round.has_value());
  const std::string path = ownBetsPathOf(round.value_or(0));
  ASSERT_EQ(send(betsCall(path, ace(), TWO_BETS_BODY))->status, EXPECTED_OK);

  const httplib::Result refused =
      send(betsCall(path, ace(), MORE_THAN_THE_WALLET_BODY));
  const httplib::Result read = send(ownCall(GET_METHOD_TEXT, path, ace()));

  ASSERT_TRUE(refused);
  EXPECT_EQ(refused->status, EXPECTED_UNPROCESSABLE_CONTENT);
  EXPECT_EQ(chipsOf(ace()), std::optional{CHIPS_AFTER_TWO_BETS});
  EXPECT_EQ(numberIn(read, STAKE_MEMBER_NAME), std::optional{TWO_BETS_STAKE});
}

/**
 * Given ACE seated
 * When ACE sends a bet of a kind no roulette has
 * Then the master answers 400, naming the kind
 */
TEST_F(CasinoRoutesFixture, ABetOfNoKindIsMalformed) {
  ASSERT_EQ(takeSeat(ace())->status, EXPECTED_CREATED);
  const std::optional<std::uint64_t> round = openRound();
  ASSERT_TRUE(round.has_value());

  const httplib::Result refused = send(
      betsCall(ownBetsPathOf(round.value_or(0)), ace(), UNKNOWN_KIND_BET_BODY));

  ASSERT_TRUE(refused);
  EXPECT_EQ(refused->status, EXPECTED_BAD_REQUEST);
}

/**
 * Given a master whose master.cfg turns its casino off
 * When it starts and a client lists the rooms
 * Then the master answers 404, and its accounts still answer
 */
TEST(CasinoRoutes, AMasterWithItsCasinoOffStarts) {
  const RunningMaster master(
      std::vector<std::string>{std::string{CASINO_MODULE_OFF_LINE}});

  const httplib::Result rooms =
      sendRouteCall(master.port(), ownCall(GET_METHOD_TEXT, ROOMS_URL, {}));
  const httplib::Result created = sendRouteCall(
      master.port(), credentialsCall(USERS_URL, aceCredentials()));

  ASSERT_TRUE(rooms);
  ASSERT_TRUE(created);
  EXPECT_EQ(rooms->status, EXPECTED_NOT_FOUND);
  EXPECT_EQ(created->status, EXPECTED_CREATED);
}

/**
 * Given ACE seated, who laid 10 on 19 and 25 on red on the round
 * When the state of the room is read again
 * Then it lists both bets of ACE, so the others seated see its chips
 */
TEST_F(CasinoRoutesFixture, TheRoomListsTheBetsOfEachPlayer) {
  ASSERT_EQ(takeSeat(ace())->status, EXPECTED_CREATED);
  const std::optional<std::uint64_t> round = openRound();
  ASSERT_TRUE(round.has_value());
  ASSERT_EQ(
      send(betsCall(ownBetsPathOf(round.value_or(0)), ace(), TWO_BETS_BODY))
          ->status,
      EXPECTED_OK);

  const httplib::Result state = takeSeat(ace());

  ASSERT_TRUE(state);
  const nlohmann::json players =
      bodyOf(state).at(std::string{PLAYERS_MEMBER_NAME});
  ASSERT_EQ(players.size(), 1U);
  EXPECT_EQ(players.front().at(std::string{BETS_MEMBER_NAME}).size(),
            TWO_BETS_COUNT);
}
