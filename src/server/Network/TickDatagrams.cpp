#include "TickDatagrams.hpp"
#include <span>
#include "IncomingDatagram.hpp"
#include "ReceivedDatagrams.hpp"

namespace rtype::server {

void TickDatagrams::begin(const ReceivedDatagrams& received) {
  received_ = received;
}

void TickDatagrams::end() { received_ = ReceivedDatagrams{}; }

std::span<const network::IncomingDatagram> TickDatagrams::players() const {
  return received_.players;
}

std::span<const network::IncomingDatagram> TickDatagrams::connectionRequests()
    const {
  return received_.connectionRequests;
}

}  // namespace rtype::server
