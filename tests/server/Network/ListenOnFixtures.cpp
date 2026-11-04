#include "ListenOnFixtures.hpp"
#include <cstdint>
#include <string>
#include "Endpoint.hpp"
#include "ListenOnTestConstants.hpp"
#include "NetworkConstants.hpp"
#include "ReceivedDatagrams.hpp"
#include "TickHandler.hpp"

rtype::server::TickHandler ignoringTicks() {
  return
      []([[maybe_unused]] const rtype::server::ReceivedDatagrams& received) {};
}

rtype::network::Endpoint loopbackEndpoint(std::uint16_t port) {
  return rtype::network::Endpoint{
      .address = std::string{rtype::network::LOOPBACK_IPV4_ADDRESS},
      .port = port};
}

rtype::network::Endpoint unusableEndpoint() {
  return rtype::network::Endpoint{.address = std::string{UNUSABLE_ADDRESS},
                                  .port = rtype::network::ANY_PORT};
}
