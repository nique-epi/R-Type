#include "DatagramSorterFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>
#include "DatagramQueues.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "RejectionReport.hpp"
#include "ServerConstants.hpp"
#include "TimeConstants.hpp"

DatagramSorterFixture::DatagramSorterFixture()
    : players_(rtype::server::INCOMING_DATAGRAM_QUEUE_CAPACITY),
      connectionRequests_(rtype::server::CONNECTION_REQUEST_QUEUE_CAPACITY),
      informationRequests_(rtype::server::INFORMATION_REQUEST_QUEUE_CAPACITY),
      sorter_(
          rtype::server::DatagramQueues{
              .players = &players_,
              .connectionRequests = &connectionRequests_,
              .informationRequests = &informationRequests_},
          rejections_, clock_) {}

void DatagramSorterFixture::sort(const rtype::network::Endpoint& sender,
                                 const std::vector<std::byte>& datagram) {
  sorter_.sort(sender, datagram);
}

void DatagramSorterFixture::sortRepeatedly(
    const rtype::network::Endpoint& sender,
    const std::vector<std::byte>& datagram, std::size_t count) {
  for (std::size_t sent = 0; sent < count; ++sent) {
    sorter_.sort(sender, datagram);
  }
}

void DatagramSorterFixture::admit(const rtype::network::Endpoint& endpoint,
                                  std::uint64_t sessionToken) {
  sorter_.admit(endpoint, sessionToken);
}

void DatagramSorterFixture::forget(const rtype::network::Endpoint& endpoint,
                                   std::uint64_t sessionToken) {
  sorter_.forget(endpoint, sessionToken);
}

void DatagramSorterFixture::advanceClock(rtype::engine::Duration elapsed) {
  clock_.advance(elapsed);
}

rtype::server::RejectionReport DatagramSorterFixture::takeReport() {
  return rejections_.takeReport();
}

std::vector<rtype::network::IncomingDatagram>
DatagramSorterFixture::drainPlayers() {
  std::vector<rtype::network::IncomingDatagram> drained;
  players_.drainInto(drained);
  return drained;
}

std::vector<rtype::network::IncomingDatagram>
DatagramSorterFixture::drainRequests() {
  std::vector<rtype::network::IncomingDatagram> drained;
  connectionRequests_.drainInto(drained);
  return drained;
}
