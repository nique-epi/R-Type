#include "RoundTripWatchFixture.hpp"
#include <algorithm>
#include <cstddef>
#include <string>
#include "Heartbeat.hpp"
#include "LobbyState.hpp"
#include "RoundTripWatch.hpp"
#include "RoundTripWatchTestConstants.hpp"
#include "ServerSessionFixtures.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

void RoundTripWatchFixture::advanceClock(engine::Duration elapsed) {
  clock_.advance(elapsed);
}

void RoundTripWatchFixture::openSession() {
  watch_.sessionOpened(
      network::LobbyPlayer{.playerNumber = OWN_PLAYER_NUMBER,
                           .ready = false,
                           .nickname = std::string{OWN_NICKNAME}},
      outbox_);
}

void RoundTripWatchFixture::hold(engine::Duration elapsed) {
  clock_.advance(elapsed);
  watch_.sessionHeld(elapsed);
}

RoundTripWatch& RoundTripWatchFixture::watch() { return watch_; }

std::size_t RoundTripWatchFixture::heartbeatsSent() const {
  return static_cast<std::size_t>(
      std::ranges::count_if(link_.sent(), holds<network::Heartbeat>));
}

network::Heartbeat RoundTripWatchFixture::lastSentHeartbeat() const {
  return onlyMessageOf<network::Heartbeat>(link_.sent().back());
}

}  // namespace rtype::client::testing
