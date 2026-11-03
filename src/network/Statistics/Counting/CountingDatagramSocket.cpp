#include "CountingDatagramSocket.hpp"
#include <cstddef>
#include <memory>
#include <span>
#include <utility>
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"
#include "TrafficCounters.hpp"

namespace rtype::network {

CountingDatagramSocket::CountingDatagramSocket(
    std::unique_ptr<IDatagramSocket> socket, TrafficCounters& counters)
    : socket_(std::move(socket)), counters_(&counters) {}

void CountingDatagramSocket::startReceiving(DatagramHandler handler) {
  socket_->startReceiving(
      [counters = counters_, handler = std::move(handler)](
          const Endpoint& sender, std::span<const std::byte> payload) {
        counters->countReceived(sender, payload);
        handler(sender, payload);
      });
}

void CountingDatagramSocket::send(const Endpoint& destination,
                                  std::span<const std::byte> payload) {
  socket_->send(destination, payload);
  counters_->countSent(destination, payload);
}

Endpoint CountingDatagramSocket::localEndpoint() const {
  return socket_->localEndpoint();
}

}  // namespace rtype::network
