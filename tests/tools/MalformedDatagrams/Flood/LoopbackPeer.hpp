#pragma once

#include <array>
#include <asio/io_context.hpp>
#include <asio/ip/udp.hpp>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include "IncomingDatagram.hpp"
#include "NetworkConstants.hpp"

namespace rtype::tools {

/**
 * @brief A UDP socket on the loopback address, which sends to other ports of
 * the same machine only: the tool cannot reach another host. A read returns
 * at once, or after a timeout it is given.
 */
class LoopbackPeer {
 public:
  /**
   * @brief Opens the socket on @p port of the loopback address; ANY_PORT lets
   * the system pick a free port.
   * @throws std::system_error, from Asio, when the system refuses the port.
   */
  explicit LoopbackPeer(std::uint16_t port);

  /**
   * @brief Sends @p datagram to @p port of the loopback address.
   * @returns Whether the system took it: a datagram it refuses is lost, as any
   * UDP datagram may be.
   */
  bool send(std::uint16_t port, std::span<const std::byte> datagram);

  /**
   * @returns The first datagram that arrives within @p timeout, or nothing.
   */
  [[nodiscard]] std::optional<network::IncomingDatagram> receive(
      std::chrono::milliseconds timeout);

  /**
   * @brief Reads every datagram waiting and throws it away.
   * @returns How many came from @p port.
   */
  std::size_t drainFrom(std::uint16_t port);

 private:
  [[nodiscard]] std::optional<network::IncomingDatagram> receiveWaiting();

  asio::io_context context_;
  asio::ip::udp::socket socket_;
  std::array<std::byte, network::RECEIVE_BUFFER_SIZE> buffer_{};
};

}  // namespace rtype::tools
