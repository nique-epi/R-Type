#include "RecordingDatagramSocket.hpp"
#include <cstddef>
#include <span>
#include <utility>
#include <vector>
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"
#include "OutgoingDatagram.hpp"

void RecordingDatagramSocket::startReceiving(
    rtype::network::DatagramHandler handler) {
  handler_ = std::move(handler);
}

void RecordingDatagramSocket::send(const rtype::network::Endpoint& destination,
                                   std::span<const std::byte> payload) {
  sent_.push_back(rtype::network::OutgoingDatagram{
      .destination = destination, .payload = {payload.begin(), payload.end()}});
}

rtype::network::Endpoint RecordingDatagramSocket::localEndpoint() const {
  return rtype::network::Endpoint{};
}

void RecordingDatagramSocket::receive(const rtype::network::Endpoint& sender,
                                      std::span<const std::byte> payload) {
  handler_(sender, payload);
}

const std::vector<rtype::network::OutgoingDatagram>&
RecordingDatagramSocket::sent() const {
  return sent_;
}
