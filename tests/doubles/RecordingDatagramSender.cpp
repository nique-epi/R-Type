#include "RecordingDatagramSender.hpp"
#include <utility>
#include <vector>
#include "OutgoingDatagram.hpp"

void RecordingDatagramSender::queueDatagram(
    rtype::network::OutgoingDatagram datagram) {
  sent_.push_back(std::move(datagram));
}

const std::vector<rtype::network::OutgoingDatagram>&
RecordingDatagramSender::sent() const {
  return sent_;
}
