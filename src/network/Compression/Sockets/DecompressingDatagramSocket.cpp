#include "DecompressingDatagramSocket.hpp"
#include <cstddef>
#include <memory>
#include <optional>
#include <span>
#include <utility>
#include <vector>
#include "DatagramCompression.hpp"
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"

namespace rtype::network {

DecompressingDatagramSocket::DecompressingDatagramSocket(
    std::unique_ptr<IDatagramSocket> socket)
    : socket_(std::move(socket)) {}

void DecompressingDatagramSocket::startReceiving(DatagramHandler handler) {
  socket_->startReceiving(
      [handler = std::move(handler)](const Endpoint& sender,
                                     std::span<const std::byte> payload) {
        if (!isCompressed(payload)) {
          handler(sender, payload);
          return;
        }
        const std::optional<std::vector<std::byte>> plain = plainForm(payload);
        if (plain.has_value()) {
          handler(sender, *plain);
        }
      });
}

void DecompressingDatagramSocket::send(const Endpoint& destination,
                                       std::span<const std::byte> payload) {
  socket_->send(destination, payload);
}

Endpoint DecompressingDatagramSocket::localEndpoint() const {
  return socket_->localEndpoint();
}

}  // namespace rtype::network
