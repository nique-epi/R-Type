#include "MasterCasinoClientFixture.hpp"
#include <chrono>
#include <optional>
#include <string>
#include <vector>
#include "BetsReceipt.hpp"
#include "CasinoPlayer.hpp"
#include "CasinoRoom.hpp"
#include "CasinoRoomListing.hpp"
#include "CasinoRoomState.hpp"
#include "FakeAccountMaster.hpp"
#include "MasterAccountClientTestConstants.hpp"
#include "MasterCasinoClient.hpp"
#include "MasterCasinoClientTestConstants.hpp"
#include "RecordedRequest.hpp"
#include "RouletteBets.hpp"
#include "RoulettePhase.hpp"
#include "RoundBets.hpp"

using rtype::network::BetsReceipt;
using rtype::network::CasinoPlayer;
using rtype::network::CasinoRoom;
using rtype::network::CasinoRoomListing;
using rtype::network::CasinoRoomState;
using rtype::network::RouletteBets;
using rtype::network::RoulettePhase;
using rtype::network::RoundBets;
using MillisecondDate = std::chrono::sys_time<std::chrono::milliseconds>;

MasterCasinoClientFixture::MasterCasinoClientFixture()
    : client_(master_.url()) {}

FakeAccountMaster& MasterCasinoClientFixture::master() { return master_; }

rtype::network::MasterCasinoClient& MasterCasinoClientFixture::client() {
  return client_;
}

RecordedRequest MasterCasinoClientFixture::lastRequest() const {
  const std::vector<RecordedRequest> requests = master_.requests();
  return requests.empty() ? RecordedRequest{} : requests.back();
}

CasinoRoomListing MasterCasinoClientFixture::testRooms() {
  const CasinoRoomState state = testRoomState();
  return CasinoRoomListing{.rooms = {CasinoRoom{.id = state.id,
                                                .name = state.name,
                                                .seats = state.seats,
                                                .seated = CASINO_TEST_SEATED,
                                                .round = state.round,
                                                .phase = state.phase}}};
}

CasinoRoomState MasterCasinoClientFixture::testRoomState() {
  return CasinoRoomState{
      .id = CASINO_TEST_ROOM,
      .name = std::string{CASINO_TEST_ROOM_NAME},
      .seats = CASINO_TEST_SEATS,
      .round = CASINO_TEST_ROUND.round,
      .phase = RoulettePhase::Betting,
      .closesAt = MillisecondDate{TEST_ROUND_CLOSES_AT},
      .resultAt = MillisecondDate{TEST_ROUND_RESULT_AT},
      .endsAt = MillisecondDate{TEST_ROUND_ENDS_AT},
      .serverTime = MillisecondDate{TEST_MASTER_TIME},
      .winningNumber = std::nullopt,
      .history = {SEVENTEEN},
      .players = {CasinoPlayer{
          .nickname = ACE.nickname, .stake = 0, .payout = std::nullopt}}};
}

RouletteBets MasterCasinoClientFixture::tenOn17() {
  return RouletteBets{.bets = {TEN_ON_17}};
}

BetsReceipt MasterCasinoClientFixture::tenOn17Receipt() {
  return BetsReceipt{.round = CASINO_TEST_ROUND.round,
                     .bets = {TEN_ON_17},
                     .stake = TEN_ON_17.amount,
                     .chips = CHIPS_LEFT_AFTER_TEN};
}

RoundBets MasterCasinoClientFixture::tenOn17Paid() {
  return RoundBets{.round = CASINO_TEST_ROUND.round,
                   .bets = {TEN_ON_17},
                   .stake = TEN_ON_17.amount,
                   .payout = TEN_ON_17_PAID,
                   .winningNumber = SEVENTEEN};
}
