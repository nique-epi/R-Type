#include "DatagramSorterFixtures.hpp"
#include <cstdint>
#include <string>
#include "Endpoint.hpp"
#include "ServerTestConstants.hpp"

namespace {

/** @returns The address the sorted datagrams come from, on @p port. */
rtype::network::Endpoint sortedSenderOn(std::uint16_t port) {
  return rtype::network::Endpoint{.address = std::string{SORTED_SENDER_ADDRESS},
                                  .port = port};
}

}  // namespace

rtype::network::Endpoint player() { return sortedSenderOn(PLAYER_PORT); }

rtype::network::Endpoint stranger() { return sortedSenderOn(STRANGER_PORT); }

rtype::network::Endpoint secondPlayer() {
  return sortedSenderOn(SECOND_PLAYER_PORT);
}
