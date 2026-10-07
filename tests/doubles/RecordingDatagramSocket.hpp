#pragma once

#include <cstddef>
#include <span>
#include <vector>
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"
#include "OutgoingDatagram.hpp"

/**
 * @brief A socket with no network behind it: the test hands it the datagrams
 * it "receives", and reads back every datagram it was asked to send.
 */
class RecordingDatagramSocket final : public rtype::network::IDatagramSocket {
 public:
  void startReceiving(rtype::network::DatagramHandler handler) override;
  void send(const rtype::network::Endpoint& destination,
            std::span<const std::byte> payload) override;
  [[nodiscard]] rtype::network::Endpoint localEndpoint() const override;

  /**
   * @brief Calls the handler given to startReceiving(), as the real socket
   * does when a datagram arrives: the bytes are only valid during the call.
   */
  void receive(const rtype::network::Endpoint& sender,
               std::span<const std::byte> payload);

  /** @returns Every datagram send() was called with, in call order. */
  [[nodiscard]] const std::vector<rtype::network::OutgoingDatagram>& sent()
      const;

 private:
  rtype::network::DatagramHandler handler_;
  std::vector<rtype::network::OutgoingDatagram> sent_;
};
