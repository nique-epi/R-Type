#include "PlayerDatagrams.hpp"
#include <span>
#include <vector>
#include "IncomingDatagram.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server {

void keepDatagramsOf(PlayerRoster& roster,
                     std::span<const network::IncomingDatagram> datagrams,
                     std::vector<network::IncomingDatagram>& kept) {
  kept.clear();
  for (const network::IncomingDatagram& datagram : datagrams) {
    if (roster.find(datagram.sender) != nullptr) {
      kept.push_back(datagram);
    }
  }
}

}  // namespace rtype::server
