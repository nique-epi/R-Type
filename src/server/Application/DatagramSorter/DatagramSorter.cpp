#include "DatagramSorter.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <span>
#include "BoundedQueue.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "MessageFrame.hpp"
#include "MessageType.hpp"
#include "NetworkException.hpp"

namespace rtype::server {

namespace {

bool holdsConnectionRequest(std::span<const network::MessageFrame> frames) {
  return std::ranges::any_of(frames, [](const network::MessageFrame& frame) {
    return frame.type ==
           static_cast<std::uint8_t>(network::MessageType::ConnectionRequest);
  });
}

}  // namespace

DatagramSorter::DatagramSorter(Destinations destinations)
    : destinations_(destinations) {}

void DatagramSorter::sort(const network::Endpoint& sender,
                          std::span<const std::byte> datagram) {
  concurrency::BoundedQueue<network::IncomingDatagram>* destination = nullptr;
  try {
    if (!network::DatagramCodec::speaksThisProtocol(
            network::DatagramCodec::decodeHeader(datagram))) {
      return;
    }
    if (holdsConnectionRequest(
            network::DatagramCodec::splitMessages(datagram))) {
      destination = destinations_.connectionRequests;
    } else if (isAdmitted(sender)) {
      destination = destinations_.players;
    }
  } catch (const network::MalformedDatagramException&) {
    return;
  }
  if (destination != nullptr) {
    destination->push(network::IncomingDatagram{
        .sender = sender, .payload = {datagram.begin(), datagram.end()}});
  }
}

void DatagramSorter::admit(const network::Endpoint& player) {
  if (!isAdmitted(player)) {
    admitted_.push_back(player);
  }
}

bool DatagramSorter::isAdmitted(const network::Endpoint& sender) const {
  return std::ranges::find(admitted_, sender) != admitted_.end();
}

}  // namespace rtype::server
