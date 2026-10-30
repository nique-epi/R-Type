#pragma once

#include <optional>
#include <vector>
#include "Endpoint.hpp"
#include "IRoutedLink.hpp"
#include "IncomingDatagram.hpp"
#include "OutgoingDatagram.hpp"

/**
 * @brief Datagram link double: it writes down every datagram sent, in order,
 * and hands over the datagrams the test delivers, in the order delivered. A
 * datagram the test never delivers is lost, as UDP may lose it. As a routed
 * link it writes down the sender it was told to listen to and drops what was
 * delivered before, as a route does; it hands over what the test delivers
 * whoever the sender is, since choosing by sender is the router's job.
 */
class RecordingDatagramLink : public rtype::client::IRoutedLink {
 public:
  /** @brief Writes the datagram down; nothing is sent. */
  void send(rtype::network::OutgoingDatagram datagram) override;
  void drainReceived(
      std::vector<rtype::network::IncomingDatagram>& destination) override;

  /** @brief Writes the sender down and drops the datagrams delivered and not
   * yet drained. */
  void listenTo(const rtype::network::Endpoint& sender) override;

  /** @brief Leaves @p datagram for the next drainReceived(). */
  void deliver(rtype::network::IncomingDatagram datagram);

  [[nodiscard]] const std::vector<rtype::network::OutgoingDatagram>& sent()
      const;

  /** @returns The sender listenTo() was last called with. */
  [[nodiscard]] const std::optional<rtype::network::Endpoint>& listenedTo()
      const;

 private:
  std::vector<rtype::network::OutgoingDatagram> sent_;
  std::optional<rtype::network::Endpoint> listenedTo_;
  std::vector<rtype::network::IncomingDatagram> delivered_;
};
