#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "ChatRelay.hpp"
#include "ConnectedPlayer.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerChatLine.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server::testing {

/** @brief A relay on a real roster, and the datagrams it sends. */
class ChatRelayFixture : public ::testing::Test {
 protected:
  ChatRelayFixture();

  /** @brief Seats the player at @p index, under a token of its own. */
  ConnectedPlayer& seat(std::size_t index);
  /** @brief Frees the seat of the player at @p index, and tells the relay, as a
   * session that closes. */
  void leave(std::size_t index);
  /** @brief The player at @p index sends the line @p lineNumber, SOME_TEXT. */
  void say(std::size_t index, std::uint16_t lineNumber);
  void runTicks(std::size_t count);

  /** @returns The lines relayed to the player at @p index, oldest first. */
  [[nodiscard]] std::vector<network::PlayerChatLine> linesTo(
      std::size_t index) const;
  [[nodiscard]] const std::vector<network::OutgoingDatagram>& sent() const;

 private:
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> sent_;
  ChatRelay relay_;
};

}  // namespace rtype::server::testing
