#include "MasterCasinoLinkFixture.hpp"
#include <chrono>
#include <functional>
#include <optional>
#include <string>
#include <thread>
#include "CasinoReadings.hpp"
#include "CasinoRoomState.hpp"
#include "FakeAccountMaster.hpp"
#include "MasterCasinoLink.hpp"
#include "MasterCasinoLinkTestConstants.hpp"
#include "RouletteBets.hpp"
#include "RoulettePhase.hpp"

namespace rtype::client::testing {

namespace {

using MillisecondDate = std::chrono::sys_time<std::chrono::milliseconds>;

}  // namespace

MasterCasinoLinkFixture::MasterCasinoLinkFixture() {
  link_.start(master_.url());
}

void MasterCasinoLinkFixture::TearDown() { link_.stop(); }

FakeAccountMaster& MasterCasinoLinkFixture::master() { return master_; }

MasterCasinoLink& MasterCasinoLinkFixture::link() { return link_; }

std::optional<CasinoReadings> MasterCasinoLinkFixture::readingsSoon(
    const std::function<bool(const CasinoReadings&)>& accepted) const {
  const auto deadline = std::chrono::steady_clock::now() + CASINO_LINK_DEADLINE;
  while (std::chrono::steady_clock::now() < deadline) {
    const CasinoReadings readings = link_.readings();
    if (accepted(readings)) {
      return readings;
    }
    std::this_thread::sleep_for(CASINO_LINK_POLL_STEP);
  }
  return std::nullopt;
}

network::CasinoRoomState MasterCasinoLinkFixture::linkRoomState() {
  return network::CasinoRoomState{
      .id = LINK_ROOM,
      .name = std::string{LINK_ROOM_NAME},
      .seats = LINK_ROOM_SEATS,
      .round = LINK_ROUND.round,
      .phase = network::RoulettePhase::Betting,
      .closesAt = MillisecondDate{LINK_CLOSES_AT},
      .resultAt = MillisecondDate{LINK_RESULT_AT},
      .endsAt = MillisecondDate{LINK_ENDS_AT},
      .serverTime = MillisecondDate{LINK_MASTER_TIME},
      .winningNumber = std::nullopt,
      .history = {},
      .players = {}};
}

network::RouletteBets MasterCasinoLinkFixture::tenOnRed() {
  return network::RouletteBets{.bets = {TEN_ON_RED}};
}

}  // namespace rtype::client::testing
