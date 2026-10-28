#include "RecordingServerLink.hpp"
#include <cstdint>
#include <utility>
#include <vector>
#include "Endpoint.hpp"
#include "OutgoingDatagram.hpp"

void RecordingServerLink::send(rtype::network::OutgoingDatagram datagram) {
  queued_.push_back(datagram);
  waiting_.push_back(std::move(datagram));
}

void RecordingServerLink::admit(const rtype::network::Endpoint& player,
                                std::uint64_t sessionToken) {
  admitted_.push_back(
      RecordedSession{.endpoint = player, .sessionToken = sessionToken});
}

void RecordingServerLink::forget(const rtype::network::Endpoint& player,
                                 std::uint64_t sessionToken) {
  forgotten_.push_back(
      RecordedSession{.endpoint = player, .sessionToken = sessionToken});
}

void RecordingServerLink::sendQueuedAfterRun() {
  for (rtype::network::OutgoingDatagram& datagram : waiting_) {
    sentAfterRun_.push_back(std::move(datagram));
  }
  waiting_.clear();
}

void RecordingServerLink::sendAtEndOfTick() { waiting_.clear(); }

const std::vector<rtype::network::OutgoingDatagram>&
RecordingServerLink::queued() const {
  return queued_;
}

const std::vector<rtype::network::OutgoingDatagram>&
RecordingServerLink::sentAfterRun() const {
  return sentAfterRun_;
}

const std::vector<RecordedSession>& RecordingServerLink::admitted() const {
  return admitted_;
}

const std::vector<RecordedSession>& RecordingServerLink::forgotten() const {
  return forgotten_;
}
