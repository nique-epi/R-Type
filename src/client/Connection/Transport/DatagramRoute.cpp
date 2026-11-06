#include "DatagramRoute.hpp"
#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>
#include "Endpoint.hpp"
#include "IDatagramLink.hpp"
#include "IDatagramPump.hpp"
#include "IncomingDatagram.hpp"
#include "OutgoingDatagram.hpp"

namespace rtype::client {

DatagramRoute::DatagramRoute(IDatagramLink& socket, IDatagramPump& pump)
    : socket_(&socket), pump_(&pump) {}

void DatagramRoute::send(network::OutgoingDatagram datagram) {
  socket_->send(std::move(datagram));
}

void DatagramRoute::drainReceived(
    std::vector<network::IncomingDatagram>& destination) {
  pump_->pump();
  std::ranges::move(received_, std::back_inserter(destination));
  received_.clear();
}

void DatagramRoute::listenTo(const network::Endpoint& sender) {
  listenToEach({sender});
}

void DatagramRoute::listenToEach(std::vector<network::Endpoint> senders) {
  pump_->pump();
  received_.clear();
  listened_ = std::move(senders);
}

bool DatagramRoute::listensTo(const network::Endpoint& sender) const {
  return std::ranges::find(listened_, sender) != listened_.end();
}

void DatagramRoute::accept(network::IncomingDatagram datagram) {
  received_.push_back(std::move(datagram));
}

}  // namespace rtype::client
