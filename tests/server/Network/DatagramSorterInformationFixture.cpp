#include "DatagramSorterInformationFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>
#include "BoundedQueue.hpp"
#include "DatagramQueues.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "RejectionReport.hpp"
#include "ServerConstants.hpp"

namespace {

std::vector<rtype::network::IncomingDatagram> drained(
    rtype::concurrency::BoundedQueue<rtype::network::IncomingDatagram>& queue) {
  std::vector<rtype::network::IncomingDatagram> datagrams;
  queue.drainInto(datagrams);
  return datagrams;
}

}  // namespace

DatagramSorterInformationFixture::DatagramSorterInformationFixture()
    : players_(rtype::server::INCOMING_DATAGRAM_QUEUE_CAPACITY),
      connectionRequests_(rtype::server::CONNECTION_REQUEST_QUEUE_CAPACITY),
      informationRequests_(rtype::server::INFORMATION_REQUEST_QUEUE_CAPACITY),
      probes_(rtype::server::PROBE_QUEUE_CAPACITY),
      sorter_(
          rtype::server::DatagramQueues{
              .players = &players_,
              .connectionRequests = &connectionRequests_,
              .informationRequests = &informationRequests_,
              .probes = &probes_},
          rejections_, clock_) {}

void DatagramSorterInformationFixture::sort(
    const rtype::network::Endpoint& sender,
    const std::vector<std::byte>& datagram) {
  sorter_.sort(sender, datagram);
}

void DatagramSorterInformationFixture::sortRepeatedly(
    const rtype::network::Endpoint& sender,
    const std::vector<std::byte>& datagram, std::size_t count) {
  for (std::size_t sorted = 0; sorted < count; ++sorted) {
    sorter_.sort(sender, datagram);
  }
}

void DatagramSorterInformationFixture::admit(
    const rtype::network::Endpoint& endpoint, std::uint64_t sessionToken) {
  sorter_.admit(endpoint, sessionToken);
}

std::vector<rtype::network::IncomingDatagram>
DatagramSorterInformationFixture::drainPlayers() {
  return drained(players_);
}

std::vector<rtype::network::IncomingDatagram>
DatagramSorterInformationFixture::drainRequests() {
  return drained(connectionRequests_);
}

std::vector<rtype::network::IncomingDatagram>
DatagramSorterInformationFixture::drainInformationRequests() {
  return drained(informationRequests_);
}

std::vector<rtype::network::IncomingDatagram>
DatagramSorterInformationFixture::drainProbes() {
  return drained(probes_);
}

rtype::server::RejectionReport DatagramSorterInformationFixture::takeReport() {
  return rejections_.takeReport();
}
