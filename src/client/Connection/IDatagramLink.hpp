#pragma once

#include <vector>
#include "IncomingDatagram.hpp"
#include "OutgoingDatagram.hpp"

namespace rtype::client {

/**
 * @brief Sends datagrams and hands over the ones received, without the caller
 * ever waiting on the network: NetworkThread in the client, a double in the
 * tests.
 */
class IDatagramLink {
 public:
  virtual ~IDatagramLink() = default;

  /** @brief Sends @p datagram, or leaves it to be sent, and returns at once. */
  virtual void send(network::OutgoingDatagram datagram) = 0;

  /**
   * @brief Moves every datagram received since the previous call, oldest
   * first, to the end of @p destination. Returns at once when nothing arrived.
   */
  virtual void drainReceived(
      std::vector<network::IncomingDatagram>& destination) = 0;
};

}  // namespace rtype::client
