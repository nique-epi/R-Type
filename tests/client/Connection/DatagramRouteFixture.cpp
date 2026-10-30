#include "DatagramRouteFixture.hpp"
#include "DatagramRoute.hpp"
#include "RecordingDatagramPump.hpp"

namespace rtype::client::testing {

DatagramRoute& DatagramRouteFixture::route() { return route_; }

const RecordingDatagramPump& DatagramRouteFixture::pump() const {
  return pump_;
}

}  // namespace rtype::client::testing
