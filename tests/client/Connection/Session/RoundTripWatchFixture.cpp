#include "RoundTripWatchFixture.hpp"
#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <string>
#include "GuaranteedDelivery.hpp"
#include "Heartbeat.hpp"
#include "LobbyState.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
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

void RoundTripWatchFixture::reopenAfterAMeasuredRoundTrip() {
  advanceClock(CLOCK_WHEN_THE_SESSION_OPENS);
  openSession();
  hold(ONE_HEARTBEAT_INTERVAL);
  deliver(
      network::Heartbeat{
          .sentAt = SERVER_HEARTBEAT_SENT_AT,
          .echo = network::HeartbeatEcho{.sentAt = FIRST_HEARTBEAT_SENT_AT,
                                         .heldFor = HELD_BY_THE_SERVER}},
      ECHO_ARRIVAL);
  watch_.sessionClosed();
  openSession();
}

void RoundTripWatchFixture::oweAnAcknowledgement() {
  acknowledgements_.open(SERVER_ACCEPTANCE_SEQUENCE);
  network::PacketHeader guaranteed;
  guaranteed.sequence = SERVER_GUARANTEED_SEQUENCE;
  network::markGuaranteed(guaranteed);
  ASSERT_TRUE(acknowledgements_.accept(guaranteed));
  outbox_.advance(network::ACKNOWLEDGEMENT_DELAY);
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
