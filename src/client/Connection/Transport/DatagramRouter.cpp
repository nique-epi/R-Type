#include "DatagramRouter.hpp"
#include <algorithm>
#include <utility>
#include "DatagramRoute.hpp"
#include "IDatagramLink.hpp"
#include "IncomingDatagram.hpp"

namespace rtype::client {

DatagramRouter::DatagramRouter(IDatagramLink& socket) : socket_(&socket) {}

DatagramRoute& DatagramRouter::addRoute() {
  return routes_.emplace_back(*socket_, *this);
}

void DatagramRouter::pump() {
  drained_.clear();
  socket_->drainReceived(drained_);
  for (network::IncomingDatagram& datagram : drained_) {
    const auto route = std::ranges::find_if(
        routes_, [&datagram](const DatagramRoute& candidate) {
          return candidate.listensTo(datagram.sender);
        });
    if (route != routes_.end()) {
      route->accept(std::move(datagram));
    }
  }
}

}  // namespace rtype::client
