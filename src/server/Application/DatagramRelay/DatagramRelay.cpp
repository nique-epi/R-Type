#include "DatagramRelay.hpp"
#include <cstddef>
#include <span>
#include "BoundedQueue.hpp"
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"
#include "IncomingDatagram.hpp"
#include "OutgoingDatagram.hpp"

namespace rtype::server {

DatagramRelay::DatagramRelay(
    network::IDatagramSocket& socket,
    engine::BoundedQueue<network::IncomingDatagram>& incoming,
    engine::BoundedQueue<network::OutgoingDatagram>& outgoing)
    : socket_(&socket), incoming_(&incoming), outgoing_(&outgoing) {}

void DatagramRelay::startReceiving() {
  socket_->startReceiving(
      [incoming = incoming_](const network::Endpoint& sender,
                             std::span<const std::byte> payload) {
        incoming->push(network::IncomingDatagram{
            .sender = sender, .payload = {payload.begin(), payload.end()}});
      });
}

void DatagramRelay::sendWaiting() {
  sending_.clear();
  outgoing_->drainInto(sending_);
  for (const network::OutgoingDatagram& datagram : sending_) {
    socket_->send(datagram.destination, datagram.payload);
  }
}

}  // namespace rtype::server
