#include "DatagramRouterFixture.hpp"
#include <utility>
#include <vector>
#include "DatagramRoute.hpp"
#include "DatagramRouter.hpp"
#include "IncomingDatagram.hpp"
#include "RecordingDatagramLink.hpp"

namespace rtype::client::testing {

void DatagramRouterFixture::arrive(network::IncomingDatagram datagram) {
  socket_.deliver(std::move(datagram));
}

std::vector<network::IncomingDatagram> DatagramRouterFixture::readBy(
    DatagramRoute& route) {
  std::vector<network::IncomingDatagram> read;
  route.drainReceived(read);
  return read;
}

DatagramRouter& DatagramRouterFixture::router() { return router_; }

const RecordingDatagramLink& DatagramRouterFixture::socket() const {
  return socket_;
}

}  // namespace rtype::client::testing
