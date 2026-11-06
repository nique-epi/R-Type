#include "CookieIssuerFixtures.hpp"
#include <string>
#include "Endpoint.hpp"
#include "HandshakeTestConstants.hpp"

namespace rtype::network::testing {

Endpoint sender() {
  return Endpoint{.address = std::string{SENDER_ADDRESS}, .port = SENDER_PORT};
}

}  // namespace rtype::network::testing
