#pragma once

#include <gtest/gtest.h>
#include <vector>
#include "DatagramRoute.hpp"
#include "DatagramRouter.hpp"
#include "IncomingDatagram.hpp"
#include "RecordingDatagramLink.hpp"

namespace rtype::client::testing {

/** @brief A router over a recording socket the test delivers datagrams to. */
class DatagramRouterFixture : public ::testing::Test {
 protected:
  /** @brief Leaves @p datagram in the socket, as if it had just arrived. */
  void arrive(network::IncomingDatagram datagram);

  /** @returns What @p route hands over now. */
  [[nodiscard]] static std::vector<network::IncomingDatagram> readBy(
      DatagramRoute& route);

  [[nodiscard]] DatagramRouter& router();

  [[nodiscard]] const RecordingDatagramLink& socket() const;

 private:
  RecordingDatagramLink socket_;
  DatagramRouter router_{socket_};
};

}  // namespace rtype::client::testing
