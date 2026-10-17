#pragma once

#include <vector>
#include "IDatagramLink.hpp"
#include "IncomingDatagram.hpp"
#include "OutgoingDatagram.hpp"

/**
 * @brief Datagram link double: it writes down every datagram sent, in order,
 * and hands over the datagrams the test delivers, in the order delivered. A
 * datagram the test never delivers is lost, as UDP may lose it.
 */
class RecordingDatagramLink : public rtype::client::IDatagramLink {
 public:
  /** @brief Writes the datagram down; nothing is sent. */
  void send(rtype::network::OutgoingDatagram datagram) override;
  void drainReceived(
      std::vector<rtype::network::IncomingDatagram>& destination) override;

  /** @brief Leaves @p datagram for the next drainReceived(). */
  void deliver(rtype::network::IncomingDatagram datagram);

  [[nodiscard]] const std::vector<rtype::network::OutgoingDatagram>& sent()
      const;

 private:
  std::vector<rtype::network::OutgoingDatagram> sent_;
  std::vector<rtype::network::IncomingDatagram> delivered_;
};
