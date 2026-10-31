#include "SimulatedNetworkSocket.hpp"
#include <cstddef>
#include <memory>
#include <span>
#include <utility>
#include "AsioEndpoint.hpp"
#include "DatagramsInFlight.hpp"
#include "DelayedDelivery.hpp"
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"
#include "NetworkConditions.hpp"
#include "NetworkContext.hpp"
#include "SimulatorConstants.hpp"

namespace rtype::network {

namespace {

/** @returns What hands a datagram whose delay ended to @p socket. */
DatagramHandler sendingThrough(IDatagramSocket& socket) {
  return [&socket](const Endpoint& destination,
                   std::span<const std::byte> payload) {
    socket.send(destination, payload);
  };
}

}  // namespace

SimulatedNetworkSocket::SimulatedNetworkSocket(
    NetworkContext& context, std::unique_ptr<IDatagramSocket> socket,
    const NetworkConditions& conditions)
    : context_(&context),
      conditions_(conditions),
      socket_(std::move(socket)),
      sends_(std::make_shared<DelayedDelivery>(
          context, DatagramsInFlight{conditions, SENT_DATAGRAMS_SEED},
          sendingThrough(*socket_))) {}

SimulatedNetworkSocket::~SimulatedNetworkSocket() {
  sends_->close();
  if (receives_) {
    receives_->close();
  }
}

void SimulatedNetworkSocket::startReceiving(DatagramHandler handler) {
  auto receives = std::make_shared<DelayedDelivery>(
      *context_, DatagramsInFlight{conditions_, RECEIVED_DATAGRAMS_SEED},
      std::move(handler));
  socket_->startReceiving(
      [receives](const Endpoint& sender, std::span<const std::byte> payload) {
        receives->add(sender, payload);
      });
  receives_ = std::move(receives);
}

void SimulatedNetworkSocket::send(const Endpoint& destination,
                                  std::span<const std::byte> payload) {
  static_cast<void>(sendableDestination(destination, payload));
  sends_->add(destination, payload);
}

Endpoint SimulatedNetworkSocket::localEndpoint() const {
  return socket_->localEndpoint();
}

}  // namespace rtype::network
