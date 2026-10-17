#include "RecordingDatagramLink.hpp"
#include <iterator>
#include <utility>
#include <vector>
#include "IncomingDatagram.hpp"
#include "OutgoingDatagram.hpp"

void RecordingDatagramLink::send(rtype::network::OutgoingDatagram datagram) {
  sent_.push_back(std::move(datagram));
}

void RecordingDatagramLink::drainReceived(
    std::vector<rtype::network::IncomingDatagram>& destination) {
  destination.insert(destination.end(),
                     std::make_move_iterator(delivered_.begin()),
                     std::make_move_iterator(delivered_.end()));
  delivered_.clear();
}

void RecordingDatagramLink::deliver(rtype::network::IncomingDatagram datagram) {
  delivered_.push_back(std::move(datagram));
}

const std::vector<rtype::network::OutgoingDatagram>&
RecordingDatagramLink::sent() const {
  return sent_;
}
