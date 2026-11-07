#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "LookRelay.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerLooks.hpp"
#include "PlayerRoster.hpp"
#include "ShipLook.hpp"

namespace rtype::server::testing {

/** @brief A relay on a real roster, and the datagrams it sends. */
class LookRelayFixture : public ::testing::Test {
 protected:
  LookRelayFixture();

  /** @brief Seats the player at @p index, under a token of its own. */
  ConnectedPlayer& seat(std::size_t index);
  /** @brief Frees the seat of the player at @p index, and tells the relay, as a
   * session that closes. */
  void leave(std::size_t index);
  /** @brief The player at @p index chooses @p look. */
  void choose(std::size_t index, identifiers::ShipLook look);
  /** @brief Tells the relay that a player arrived. */
  void arrive();
  void runTicks(std::size_t count);

  /** @returns The PlayerLooks sent to the player at @p index, oldest first. */
  [[nodiscard]] std::vector<network::PlayerLooks> looksTo(
      std::size_t index) const;
  [[nodiscard]] const std::vector<network::OutgoingDatagram>& sent() const;

 private:
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> sent_;
  LookRelay relay_;
};

}  // namespace rtype::server::testing
