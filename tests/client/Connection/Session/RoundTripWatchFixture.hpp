#pragma once

#include <gtest/gtest.h>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "Heartbeat.hpp"
#include "MessageDispatcher.hpp"
#include "MessageFrame.hpp"
#include "MessageOrigin.hpp"
#include "MessagePayload.hpp"
#include "RecordingDatagramLink.hpp"
#include "RoundTripWatch.hpp"
#include "SessionOutbox.hpp"
#include "SimulatedClock.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief A round trip watch over a session outbox to a recording link, and
 * a clock the test moves. */
class RoundTripWatchFixture : public ::testing::Test {
 protected:
  /** @brief Moves the clock by @p elapsed, without a session. */
  void advanceClock(engine::Duration elapsed);

  /** @brief Opens the session of OWN_PLAYER_NUMBER over the outbox. */
  void openSession();

  /** @brief Moves the clock by @p elapsed, then lets a frame of the session
   * go by. */
  void hold(engine::Duration elapsed);

  /** @brief Hands the dispatcher @p message, as a datagram of the server that
   * arrived at @p receivedAt. */
  template <network::EncodableMessage Message>
  void deliver(const Message& message, std::chrono::nanoseconds receivedAt) {
    const std::vector<std::byte> payload = network::encodeMessage(message);
    const std::vector frames{network::MessageFrame{
        .type = static_cast<std::uint8_t>(Message::TYPE), .payload = payload}};
    dispatcher_.dispatch(
        network::MessageOrigin{
            .sender = {}, .header = {}, .receivedAt = receivedAt},
        frames);
  }

  [[nodiscard]] RoundTripWatch& watch();

  /** @returns How many Heartbeats went out. */
  [[nodiscard]] std::size_t heartbeatsSent() const;

  /** @returns The newest datagram sent, read as its only Heartbeat. */
  [[nodiscard]] network::Heartbeat lastSentHeartbeat() const;

 private:
  network::MessageDispatcher dispatcher_;
  SimulatedClock clock_;
  RoundTripWatch watch_{dispatcher_, clock_};
  RecordingDatagramLink link_;
  SessionOutbox outbox_{link_};
};

}  // namespace rtype::client::testing
