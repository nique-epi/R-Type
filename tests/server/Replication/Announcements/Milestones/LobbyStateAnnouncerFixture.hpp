#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "LobbyStateAnnouncer.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "ServerLobbyTestConstants.hpp"

namespace rtype::server::testing {

/** @brief An announcer over a roster, whose datagrams are recorded instead of
 * sent. */
class LobbyStateAnnouncerFixture : public ::testing::Test {
 protected:
  /** @brief Connects the player number @p index + 1, under @p sessionToken. */
  ConnectedPlayer& join(std::size_t index,
                        std::uint64_t sessionToken = SESSION_TOKEN);

  /** @brief Lets @p count ticks of the waiting room pass. */
  void runWaitingTicks(std::size_t count);

  /** @returns The datagrams sent to the player at @p index, oldest first. */
  [[nodiscard]] std::vector<network::OutgoingDatagram> sentTo(
      std::size_t index) const;

  [[nodiscard]] const std::vector<network::OutgoingDatagram>& sent() const;

 private:
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> sent_;
  LobbyStateAnnouncer announcer_{roster_,
                                 [this](network::OutgoingDatagram datagram) {
                                   sent_.push_back(std::move(datagram));
                                 }};
};

}  // namespace rtype::server::testing
