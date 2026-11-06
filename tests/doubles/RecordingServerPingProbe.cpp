#include "RecordingServerPingProbe.hpp"
#include <utility>
#include <vector>
#include "Endpoint.hpp"
#include "PingAnswer.hpp"

void RecordingServerPingProbe::measure(
    std::vector<rtype::network::Endpoint> servers) {
  rounds_.push_back(std::move(servers));
  delivered_.clear();
}

std::vector<rtype::client::PingAnswer> RecordingServerPingProbe::answers() {
  return std::exchange(delivered_, {});
}

void RecordingServerPingProbe::deliver(rtype::client::PingAnswer answer) {
  delivered_.push_back(std::move(answer));
}

const std::vector<std::vector<rtype::network::Endpoint>>&
RecordingServerPingProbe::rounds() const {
  return rounds_;
}
