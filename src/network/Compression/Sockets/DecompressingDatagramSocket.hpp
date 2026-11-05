#pragma once

#include <cstddef>
#include <memory>
#include <span>
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"

namespace rtype::network {

/**
 * @brief The socket of the client: decodes the body of the datagrams it
 * receives, over another socket it owns.
 *
 * It sends everything as it is. A datagram received with COMPRESSED_FLAG
 * raised is decoded and handed on with the flag cleared, so nothing above
 * sees the coding; one that does not decode, or that carries the token 0, is
 * dropped. The decoder never produces more than MAXIMUM_BODY_SIZE bytes and
 * keeps nothing between two datagrams, so a forged datagram costs a few
 * microseconds and nothing else.
 *
 * Like any socket, use it on the thread that runs its NetworkContext.
 */
class DecompressingDatagramSocket final : public IDatagramSocket {
 public:
  explicit DecompressingDatagramSocket(std::unique_ptr<IDatagramSocket> socket);

  /** @brief Hands each datagram received to @p handler, decoded when it was
   * coded.
   * @throws What the wrapped socket throws. */
  void startReceiving(DatagramHandler handler) override;

  /** @brief Hands @p payload to the wrapped socket as it is.
   * @throws What the wrapped socket throws. */
  void send(const Endpoint& destination,
            std::span<const std::byte> payload) override;

  /** @returns The address and port of the wrapped socket. */
  [[nodiscard]] Endpoint localEndpoint() const override;

 private:
  std::unique_ptr<IDatagramSocket> socket_;
};

}  // namespace rtype::network
