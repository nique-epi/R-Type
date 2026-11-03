#pragma once

#include <optional>
#include <vector>
#include "Endpoint.hpp"
#include "IDatagramLink.hpp"
#include "IDatagramPump.hpp"
#include "IRoutedLink.hpp"
#include "IncomingDatagram.hpp"
#include "OutgoingDatagram.hpp"

namespace rtype::client {

/**
 * @brief The share of a socket that one recipient reads: the datagrams of the
 * sender it listens to, and nothing before listenTo() names one. What it
 * sends goes straight to the socket.
 *
 * A DatagramRouter makes it and fills it; its recipient only reads. Not
 * thread-safe: use it on the thread of the frame loop.
 */
class DatagramRoute final : public IRoutedLink {
 public:
  /**
   * @param socket Where the datagrams go; it must outlive the route.
   * @param pump Asked to hand out what the socket received before each read;
   * it must outlive the route.
   */
  DatagramRoute(IDatagramLink& socket, IDatagramPump& pump);

  void send(network::OutgoingDatagram datagram) override;

  /**
   * @brief Moves the datagrams of the sender listened to, received since the
   * previous call, oldest first, to the end of @p destination.
   * @throws What the socket throws when it drains.
   */
  void drainReceived(
      std::vector<network::IncomingDatagram>& destination) override;

  void listenTo(const network::Endpoint& sender) override;

  /** @returns Whether the datagrams of @p sender are for this route. */
  [[nodiscard]] bool listensTo(const network::Endpoint& sender) const;

  /** @brief Keeps @p datagram for the next drainReceived(). */
  void accept(network::IncomingDatagram datagram);

 private:
  IDatagramLink* socket_;
  IDatagramPump* pump_;
  std::optional<network::Endpoint> listened_;
  std::vector<network::IncomingDatagram> received_;
};

}  // namespace rtype::client
