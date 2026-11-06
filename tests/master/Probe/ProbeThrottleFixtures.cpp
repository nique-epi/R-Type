#include "ProbeThrottleFixtures.hpp"
#include <cstdint>
#include <string>
#include "Endpoint.hpp"
#include "ProbeThrottleTestConstants.hpp"

rtype::network::Endpoint throttledDestination(std::uint16_t port) {
  return rtype::network::Endpoint{.address = std::string{THROTTLED_ADDRESS},
                                  .port = port};
}
