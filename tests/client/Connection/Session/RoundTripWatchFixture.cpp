#include "RoundTripWatchFixture.hpp"
#include <string>
#include "LobbyState.hpp"
#include "RecordingSessionOutbox.hpp"
#include "RoundTripWatch.hpp"
#include "RoundTripWatchTestConstants.hpp"
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

const RecordingSessionOutbox& RoundTripWatchFixture::outbox() const {
  return outbox_;
}

}  // namespace rtype::client::testing
