#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "Application.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "MessageType.hpp"
#include "RecordingServerLink.hpp"
#include "ServerApplication.hpp"

namespace rtype::server::testing {

/**
 * @brief The games and the lobby of the server, started in an Application
 * after a ServerTrafficModule that stands for the network: the test hands the
 * datagrams of each tick, runs the loop itself, and reads what the modules
 * asked of the network in a RecordingServerLink.
 *
 * The games read the waves file shipped in the assets of the repository.
 */
class ServerModulesFixture : public ::testing::Test {
 protected:
  ServerModulesFixture();

  /** @brief Runs one tick of the loop, the players having sent @p players and
   * strangers @p requests. */
  void tick(std::vector<network::IncomingDatagram> players,
            std::vector<network::IncomingDatagram> requests);

  /**
   * @brief Has the player at playerEndpoint(@p index) ask to connect, then ask
   * again with the cookie of the challenge it got, as a client does.
   */
  void seatPlayer(std::size_t index = 0);

  [[nodiscard]] RecordingServerLink& link();
  [[nodiscard]] engine::Application& application();

  /** @returns The type of each datagram queued for @p destination, oldest
   * first. */
  [[nodiscard]] std::vector<network::MessageType> messageTypesQueuedFor(
      const network::Endpoint& destination) const;

  /** @returns The type of each datagram sent to @p destination once the loop
   * stopped, oldest first. */
  [[nodiscard]] std::vector<network::MessageType> messageTypesSentAfterRunTo(
      const network::Endpoint& destination) const;

  /** @returns The address of each session the network was told to admit,
   * oldest first. */
  [[nodiscard]] std::vector<network::Endpoint> admittedEndpoints() const;

 private:
  RecordingServerLink link_;
  ServerApplication::ReceivedDatagrams received_{};
  std::vector<network::IncomingDatagram> players_;
  std::vector<network::IncomingDatagram> requests_;
  engine::Application application_;
};

}  // namespace rtype::server::testing
