#include "DatagramRelay.hpp"
#include <cstddef>
#include <span>
#include "BoundedQueue.hpp"
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"
#include "IncomingDatagram.hpp"
#include "OutgoingDatagram.hpp"

namespace rtype::network {

DatagramRelay::DatagramRelay(
    IDatagramSocket& socket,
    concurrency::BoundedQueue<IncomingDatagram>& incoming,
    concurrency::BoundedQueue<OutgoingDatagram>& outgoing)
    : socket_(&socket), incoming_(&incoming), outgoing_(&outgoing) {}

void DatagramRelay::startReceiving() {
  socket_->startReceiving(
      [incoming = incoming_](const Endpoint& sender,
                             std::span<const std::byte> payload) {
        incoming->push(IncomingDatagram{
            .sender = sender, .payload = {payload.begin(), payload.end()}});
      });
}

void DatagramRelay::sendWaiting() {
  sending_.clear();
  outgoing_->drainInto(sending_);
  for (const OutgoingDatagram& datagram : sending_) {
    socket_->send(datagram.destination, datagram.payload);
  }
}

}  // namespace rtype::network
