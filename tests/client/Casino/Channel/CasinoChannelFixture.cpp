#include "CasinoChannelFixture.hpp"
#include <chrono>
#include <optional>
#include <string>
#include "BetsReading.hpp"
#include "BetsReceipt.hpp"
#include "CasinoChannel.hpp"
#include "CasinoChannelTestConstants.hpp"
#include "CasinoRoomReading.hpp"
#include "CasinoRoomState.hpp"
#include "FakeAccountSession.hpp"
#include "FakeWallet.hpp"
#include "MillisecondDate.hpp"
#include "RecordingCasinoLink.hpp"
#include "RoomRound.hpp"
#include "RoulettePhase.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

namespace {

using network::MillisecondDate;

}  // namespace

CasinoChannelFixture::CasinoChannelFixture()
    : channel_(link_, session_, wallet_) {
  session_.signInAs(CHANNEL_ACCOUNT_ID, std::string{CHANNEL_TOKEN});
}

RecordingCasinoLink& CasinoChannelFixture::link() { return link_; }

FakeAccountSession& CasinoChannelFixture::session() { return session_; }

FakeWallet& CasinoChannelFixture::wallet() { return wallet_; }

CasinoChannel& CasinoChannelFixture::channel() { return channel_; }

void CasinoChannelFixture::wait(engine::Duration elapsed) {
  channel_.update(elapsed);
}

void CasinoChannelFixture::answerTheRead(
    const network::CasinoRoomState& state) {
  link_.answers.room = CasinoRoomReading{
      .ask = link_.roomsRead.size(), .room = state.id, .state = state};
}

network::CasinoRoomState CasinoChannelFixture::roomAt(
    std::chrono::milliseconds masterTime) {
  return network::CasinoRoomState{
      .id = CHANNEL_ROOM,
      .name = std::string{},
      .seats = 0,
      .round = CHANNEL_ROUND,
      .phase = network::RoulettePhase::Betting,
      .closesAt = MillisecondDate{CHANNEL_CLOSES_AT},
      .resultAt = MillisecondDate{CHANNEL_RESULT_AT},
      .endsAt = MillisecondDate{CHANNEL_ENDS_AT},
      .serverTime = MillisecondDate{masterTime},
      .winningNumber = std::nullopt,
      .history = {},
      .players = {}};
}

BetsReading CasinoChannelFixture::tenOnBlackTaken() {
  return BetsReading{
      .ask = 1,
      .round = network::RoomRound{.room = CHANNEL_ROOM, .round = CHANNEL_ROUND},
      .receipt = network::BetsReceipt{.round = CHANNEL_ROUND,
                                      .bets = {TEN_ON_BLACK},
                                      .stake = TEN_ON_BLACK.amount,
                                      .chips = CHANNEL_CHIPS_LEFT},
      .refusal = std::nullopt};
}

}  // namespace rtype::client::testing
