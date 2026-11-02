#include "MessageDeliveryFixtures.hpp"
#include "EncodedMessage.hpp"
#include "EntityDestroyed.hpp"
#include "MessageDeliveryTestConstants.hpp"
#include "MessagePayload.hpp"
#include "PlayerLeft.hpp"

namespace rtype::server::testing {

network::EncodedMessage playerLeftMessage() {
  return network::encodedMessage(SECOND_PLAYER_LEFT);
}

network::EncodedMessage entityDestroyedMessage() {
  return network::encodedMessage(MONSTER_KILLED);
}

}  // namespace rtype::server::testing
