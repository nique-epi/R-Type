#include "MessageDeliveryFixtures.hpp"
#include "EncodedMessage.hpp"
#include "MessageDeliveryTestConstants.hpp"
#include "MessagePayload.hpp"

namespace rtype::server::testing {

network::EncodedMessage playerLeftMessage() {
  return network::encodedMessage(SECOND_PLAYER_LEFT);
}

network::EncodedMessage entityDestroyedMessage() {
  return network::encodedMessage(MONSTER_KILLED);
}

}  // namespace rtype::server::testing
