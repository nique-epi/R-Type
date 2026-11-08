#pragma once

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "Ban.hpp"
#include "ConnectionAcceptor.hpp"
#include "Endpoint.hpp"
#include "LatestBans.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server::testing {

/** @brief An acceptor given bans the test publishes, its answers recorded. */
class BanningAcceptorFixture : public ::testing::Test {
 protected:
  BanningAcceptorFixture();

  /** @brief Publishes @p bans, as the storage module does once it read them. */
  void publish(std::vector<Ban> bans);

  /** @brief Receives a request of @p sender for @p nickname, without a cookie,
   * at REQUEST_TIME. */
  void request(const network::Endpoint& sender, const std::string& nickname);

  /** @returns The last answer; a failure of the test when there is none. */
  [[nodiscard]] const network::OutgoingDatagram& lastAnswer() const;

 private:
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> answers_;
  LatestBans bans_;
  ConnectionAcceptor acceptor_;
};

}  // namespace rtype::server::testing
