#include "DatagramSorterFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "TimeConstants.hpp"

void DatagramSorterFixture::sort(const rtype::network::Endpoint& sender,
                                 const std::vector<std::byte>& datagram) {
  sorter_.sort(sender, datagram);
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

std::size_t DatagramSorterFixture::droppedCount() const {
  return sorter_.droppedCount();
}

std::size_t DatagramSorterFixture::wrongTokenCount() const {
  return sorter_.wrongTokenCount();
}

std::optional<rtype::network::Endpoint>
DatagramSorterFixture::takeWrongTokenSender() {
  return sorter_.takeWrongTokenSender();
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
