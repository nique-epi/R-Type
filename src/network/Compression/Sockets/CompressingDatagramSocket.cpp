#include "CompressingDatagramSocket.hpp"
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

CompressingDatagramSocket::CompressingDatagramSocket(
    std::unique_ptr<IDatagramSocket> socket)
    : socket_(std::move(socket)) {}

void CompressingDatagramSocket::startReceiving(DatagramHandler handler) {
  socket_->startReceiving(
      [handler = std::move(handler)](const Endpoint& sender,
                                     std::span<const std::byte> payload) {
        if (!isCompressed(payload)) {
          handler(sender, payload);
        }
      });
}

void CompressingDatagramSocket::send(const Endpoint& destination,
                                     std::span<const std::byte> payload) {
  const std::optional<std::vector<std::byte>> compressed =
      compressedForm(payload);
  socket_->send(destination, compressed.has_value()
                                 ? std::span<const std::byte>{*compressed}
                                 : payload);
}

Endpoint CompressingDatagramSocket::localEndpoint() const {
  return socket_->localEndpoint();
}

}  // namespace rtype::network
