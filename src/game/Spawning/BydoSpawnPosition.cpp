#include "BydoSpawnPosition.hpp"
#include "GameException.hpp"
#include "PlayfieldConstants.hpp"
#include "Position.hpp"
#include "SpawningConstants.hpp"

namespace rtype::game {

Position bydoSpawnPosition(float randomUnit) {
  const bool isUnit = randomUnit >= 0.0F && randomUnit < 1.0F;
  if (!isUnit) {
    throw InvalidRandomUnitException();
  }
  const float rowsHeight =
      PLAY_AREA_HEIGHT - (2.0F * BYDO_SPAWN_VERTICAL_MARGIN);
  return Position{.x = PLAYFIELD_WIDTH + BYDO_SPAWN_MARGIN,
                  .y = BYDO_SPAWN_VERTICAL_MARGIN + (rowsHeight * randomUnit)};
}

}  // namespace rtype::game
