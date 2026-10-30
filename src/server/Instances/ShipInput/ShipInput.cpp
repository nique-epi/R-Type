#include "ShipInput.hpp"
#include <cstdint>
#include "ConnectedPlayer.hpp"
#include "Intent.hpp"
#include "PlayerInput.hpp"
#include "ProtocolConstants.hpp"
#include "ShipControl.hpp"
#include "ShipDirections.hpp"
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

namespace {

/**
 * @returns The directions the player held before @p now arrived: those of the
 * Intent of its ship when an input already steered it. A ship no input reached
 * yet knows nothing: when the player sent inputs before, to an earlier ship,
 * @p now is taken for what it already held, so a key held through a respawn
 * presses nothing; the first input of a session presses what it holds.
 */
game::ShipDirections heldBefore(const ConnectedPlayer& player,
                                const game::Intent* held,
                                const game::ShipDirections& now) {
  if (held != nullptr && held->steered) {
    return held->directions;
  }
  return player.inputApplied ? now : game::ShipDirections{};
}

}  // namespace

void noteTaps(game::World& world, const ConnectedPlayer& player,
              const network::PlayerInput& input) {
  game::ShipMotion* const motion = world.shipMotionOf(player.shipEntityId);
  if (motion == nullptr) {
    return;
  }
  const game::Intent* const held =
      world.shipComponentOf<game::Intent>(player.shipEntityId);
  const game::ShipDirections now = directionsOf(input);
  const game::ShipDirections before = heldBefore(player, held, now);
  *motion = game::motionAfterInput(
      *motion,
      game::HeldChange{
          .before = before, .now = now, .inputNumber = input.inputNumber});
}

}  // namespace rtype::server
