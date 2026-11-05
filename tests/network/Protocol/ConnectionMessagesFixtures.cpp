#include "ConnectionMessagesFixtures.hpp"
#include <string>
#include "ConnectionRequest.hpp"
#include "ProtocolTestConstants.hpp"

namespace rtype::network::testing {

ConnectionRequest smallestRequest() {
  return ConnectionRequest{.cookie = 0,
                           .nickname = std::string{ONE_BYTE_NICKNAME}};
}

}  // namespace rtype::network::testing
