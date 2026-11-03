#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "ConnectionAcceptor.hpp"
#include "Endpoint.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::server::testing {

/** @brief An acceptor whose answers are recorded instead of sent. */
class ConnectionAcceptorFixture : public ::testing::Test {
 protected:
  void receive(const network::Endpoint& sender, std::vector<std::byte> datagram,
               engine::Duration now = REQUEST_TIME);

  /** @returns The cookie of the challenge @p sender gets for a request
   * without one. */
  std::uint64_t challengeCookie(const network::Endpoint& sender,
                                const std::string& nickname);

  std::uint64_t challengeCookie(const network::Endpoint& sender);

  /** @brief Goes through the challenge and sends the request with its
   * cookie. */
  void connect(const network::Endpoint& sender);

  /** @brief Goes through the challenge and the request with its cookie, for a
   * player who asks for @p nickname. */
  void connectAs(const network::Endpoint& sender, const std::string& nickname);

  [[nodiscard]] const std::vector<network::OutgoingDatagram>& answers() const;

  [[nodiscard]] const network::OutgoingDatagram& lastAnswer() const;

  [[nodiscard]] PlayerRoster& roster();

  [[nodiscard]] const std::vector<std::pair<network::Endpoint, std::uint64_t>>&
  admitted() const;

  [[nodiscard]] ConnectionAcceptor& acceptor();

  /** @returns The numbers of the players the acceptor reported as joined. */
  [[nodiscard]] const std::vector<std::uint8_t>& joinedPlayers() const;

 private:
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> answers_;
  std::vector<std::pair<network::Endpoint, std::uint64_t>> admitted_;
  std::vector<std::uint8_t> joinedPlayers_;
  ConnectionAcceptor acceptor_{
      roster_,
      [this](network::OutgoingDatagram datagram) {
        answers_.push_back(std::move(datagram));
      },
      [this](const network::Endpoint& player, std::uint64_t sessionToken) {
        admitted_.emplace_back(player, sessionToken);
      },
      [this](ConnectedPlayer& player) {
        joinedPlayers_.push_back(player.playerNumber);
      }};
};

}  // namespace rtype::server::testing
