#include "ShipInput.hpp"
#include <cstdint>
#include "ConnectedPlayer.hpp"
#include "PlayerInput.hpp"
#include "ProtocolConstants.hpp"
#include "ShipControl.hpp"
#include "ShipMotion.hpp"
#include "World.hpp"

namespace rtype::server {

bool holdsActionAt(const network::PlayerInput& input,
                   std::uint8_t bitPosition) {
  const unsigned int mask = 1U << static_cast<unsigned int>(bitPosition);
  return (static_cast<unsigned int>(input.actions) & mask) != 0U;
}

game::ShipDirections directionsOf(const network::PlayerInput& input) {
  return game::ShipDirections{
      .up = holdsActionAt(input, network::UP_ACTION_BIT_POSITION),
      .down = holdsActionAt(input, network::DOWN_ACTION_BIT_POSITION),
      .left = holdsActionAt(input, network::LEFT_ACTION_BIT_POSITION),
      .right = holdsActionAt(input, network::RIGHT_ACTION_BIT_POSITION)};
}

void noteTaps(game::World& world, const ConnectedPlayer& player,
              const network::PlayerInput& input) {
  game::ShipMotion* const motion = world.shipMotionOf(player.shipEntityId);
  if (motion == nullptr) {
    return;
  }
  const game::ShipDirections before = player.newestInput.has_value()
                                          ? directionsOf(*player.newestInput)
                                          : game::ShipDirections{};
  *motion = game::motionAfterInput(
      *motion, game::HeldChange{.before = before,
                                .now = directionsOf(input),
                                .inputNumber = input.inputNumber});
}

}  // namespace rtype::server
