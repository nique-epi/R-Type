#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "ConnectionAcceptor.hpp"
#include "DebugJournal.hpp"
#include "Endpoint.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::server::testing {

/**
 * @brief An acceptor whose answers are recorded instead of sent, with the log
 * written to a journal the test reads.
 */
class ConnectionAcceptorFixture : public ::testing::Test {
 protected:
  ConnectionAcceptorFixture();

  void receive(const network::Endpoint& sender, std::vector<std::byte> datagram,
               engine::Duration now = REQUEST_TIME);

  /** @returns The cookie of the challenge @p sender gets for a request
   * without one, asking for @p nickname. */
  std::uint64_t challengeCookie(const network::Endpoint& sender,
                                const std::string& nickname = std::string{
                                    PLAYER_NICKNAME});

  /** @brief Goes through the challenge and sends the request with its
   * cookie, for a player who asks for @p nickname. */
  void connect(const network::Endpoint& sender,
               const std::string& nickname = std::string{PLAYER_NICKNAME});

  [[nodiscard]] const std::vector<network::OutgoingDatagram>& answers() const;

  [[nodiscard]] const network::OutgoingDatagram& lastAnswer() const;

  [[nodiscard]] PlayerRoster& roster();

  [[nodiscard]] const std::vector<std::pair<network::Endpoint, std::uint64_t>>&
  admitted() const;

  [[nodiscard]] ConnectionAcceptor& acceptor();

  /** @returns The numbers of the players the acceptor reported as joined. */
  [[nodiscard]] const std::vector<std::uint8_t>& joinedPlayers() const;

  /** @returns The lines of the journal that hold @p text, oldest first. */
  [[nodiscard]] std::vector<std::string> journalLinesHolding(
      std::string_view text) const;

 private:
  DebugJournal journal_;
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> answers_;
  std::vector<std::pair<network::Endpoint, std::uint64_t>> admitted_;
  std::vector<std::uint8_t> joinedPlayers_;
  ConnectionAcceptor acceptor_;
};

}  // namespace rtype::server::testing
