#include "DatagramSorter.hpp"
#include <algorithm>
#include <cstddef>
#include <optional>
#include <span>
#include "BoundedQueue.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "MessageType.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::server {

DatagramSorter::DatagramSorter(Destinations destinations)
    : destinations_(destinations) {}

void DatagramSorter::sort(const network::Endpoint& sender,
                          std::span<const std::byte> datagram) {
  concurrency::BoundedQueue<network::IncomingDatagram>* const destination =
      destinationOf(sender, datagram);
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

concurrency::BoundedQueue<network::IncomingDatagram>*
DatagramSorter::destinationOf(const network::Endpoint& sender,
                              std::span<const std::byte> datagram) const {
  if (datagram.size() < network::PACKET_HEADER_SIZE ||
      !network::DatagramCodec::speaksThisProtocol(
          network::DatagramCodec::decodeHeader(datagram))) {
    return nullptr;
  }
  const std::optional<network::DatagramCodec::MessageTypes> types =
      network::DatagramCodec::messageTypes(datagram);
  if (!types.has_value()) {
    return nullptr;
  }
  if (types->test(
          static_cast<std::size_t>(network::MessageType::ConnectionRequest))) {
    return destinations_.connectionRequests;
  }
  if (isAdmitted(sender)) {
    return destinations_.players;
  }
  return nullptr;
}

bool DatagramSorter::isAdmitted(const network::Endpoint& sender) const {
  return std::ranges::find(admitted_, sender) != admitted_.end();
}

}  // namespace rtype::server
