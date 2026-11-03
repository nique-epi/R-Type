#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>
#include "EncodedMessage.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server::testing {

/** @brief Two players, FIRST_PLAYER and SECOND_PLAYER, and the datagrams sent
 * to them, in order. */
class MessageDeliveryFixture : public ::testing::Test {
 protected:
  MessageDeliveryFixture();

  [[nodiscard]] PlayerRoster& roster();
  void sendOnce(const network::EncodedMessage& message);
  void guarantee(const network::EncodedMessage& message);

  /** @brief Runs @p count ticks of the server: each makes the guaranteed
   * messages one tick older, as the start of a tick does, then resends what
   * is due, as its end does. */
  void endTicks(std::size_t count);

  /** @returns The datagrams sent to the player at @p index, in order. */
  [[nodiscard]] std::vector<network::OutgoingDatagram> sentTo(
      std::size_t index) const;

  /** @brief Has the player at @p index acknowledge the datagram of
   * @p sequence it got, as GameSessions does with a datagram of the player. */
  void acknowledgeFrom(std::size_t index, std::uint16_t sequence);

 private:
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> sent_;
  std::function<void(network::OutgoingDatagram)> send_;
};

}  // namespace rtype::server::testing
