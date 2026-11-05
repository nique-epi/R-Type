#pragma once

#include <cstddef>
#include <memory>
#include <span>
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"

namespace rtype::network {

/**
 * @brief The socket of the server: codes the body of the datagrams it sends,
 * over another socket it owns.
 *
 * It is the end of the network, so put it right over the network simulator,
 * which then loses and delays the coded datagrams as a real network would,
 * and under the socket that counts, which then sees the datagrams as the
 * messages left them.
 *
 * A client never codes what it sends: a datagram received with COMPRESSED_FLAG
 * raised is dropped, so the server decodes nothing a stranger sent.
 *
 * Like any socket, use it on the thread that runs its NetworkContext.
 */
class CompressingDatagramSocket final : public IDatagramSocket {
 public:
  explicit CompressingDatagramSocket(std::unique_ptr<IDatagramSocket> socket);

  /** @brief Hands each datagram received to @p handler, but those with
   * COMPRESSED_FLAG raised.
   * @throws What the wrapped socket throws. */
  void startReceiving(DatagramHandler handler) override;

  /** @brief Sends @p payload with its body coded when that makes it shorter,
   * as it is otherwise.
   * @throws What the wrapped socket throws. */
  void send(const Endpoint& destination,
            std::span<const std::byte> payload) override;

  /** @returns The address and port of the wrapped socket. */
  [[nodiscard]] Endpoint localEndpoint() const override;

 private:
  std::unique_ptr<IDatagramSocket> socket_;
};

}  // namespace rtype::network
