#include "TickDatagrams.hpp"
#include <span>
#include "IncomingDatagram.hpp"
#include "ServerApplication.hpp"

namespace rtype::server {

void TickDatagrams::begin(
    const ServerApplication::ReceivedDatagrams& received) {
  received_ = received;
}

void TickDatagrams::end() {
  received_ = ServerApplication::ReceivedDatagrams{};
}

std::span<const network::IncomingDatagram> TickDatagrams::players() const {
  return received_.players;
}

std::span<const network::IncomingDatagram> TickDatagrams::connectionRequests()
    const {
  return received_.connectionRequests;
}

}  // namespace rtype::server
