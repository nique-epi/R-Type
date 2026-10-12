#pragma once

#include <cstddef>
#include <memory>
#include <span>
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"
#include "NetworkContext.hpp"

namespace rtype::network {

/**
 * @brief A UDP socket that runs on the event loop of a NetworkContext.
 *
 * Its Asio state lives in an implementation shared with the operations in
 * flight: an operation that completes after the socket is destroyed touches no
 * freed memory, and never calls the handler.
 */
class UdpSocket final : public IDatagramSocket {
 public:
  /**
   * @brief Opens a socket bound to @p localEndpoint. Port 0 lets the system
   *        pick a free port.
   *
   * @throws InvalidAddressException when the address of @p localEndpoint is
   *         not an IP address.
   * @throws SocketOpenException when the system refuses to open or bind the
   *         socket, for example because the port is already taken.
   */
  UdpSocket(NetworkContext& context, const Endpoint& localEndpoint);

  /**
   * @brief Closes the socket. The operations in flight are cancelled, and the
   *        handler is never called again.
   */
  ~UdpSocket() override;

  UdpSocket(const UdpSocket&) = delete;
  UdpSocket& operator=(const UdpSocket&) = delete;
  UdpSocket(UdpSocket&&) = delete;
  UdpSocket& operator=(UdpSocket&&) = delete;

  void startReceiving(DatagramHandler handler) override;
  void send(const Endpoint& destination,
            std::span<const std::byte> payload) override;
  [[nodiscard]] Endpoint localEndpoint() const override;

 private:
  struct Implementation;

  std::shared_ptr<Implementation> implementation_;
};

}  // namespace rtype::network
