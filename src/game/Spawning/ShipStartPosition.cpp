#include "ShipStartPosition.hpp"
#include <cstddef>
#include <cstdint>
#include "GameException.hpp"
#include "PlayfieldConstants.hpp"
#include "Position.hpp"
#include "SpawningConstants.hpp"

namespace rtype::game {

Position shipStartPosition(std::uint8_t playerNumber, std::size_t playerCount) {
  if (playerNumber == 0 || playerNumber > playerCount) {
    throw InvalidPlayerNumberException();
  }
  const float rowSpacing =
      PLAYFIELD_HEIGHT / static_cast<float>(playerCount + 1);
  return Position{.x = PLAYFIELD_WIDTH * SHIP_START_X_RATIO,
                  .y = rowSpacing * static_cast<float>(playerNumber)};
}

}  // namespace rtype::game
