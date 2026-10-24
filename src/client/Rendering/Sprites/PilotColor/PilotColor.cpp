#include "PilotColor.hpp"
#include "EntitySpriteConstants.hpp"

namespace rtype::client {

bool isPilotColor(int color) {
  return color >= 0 && color < PLAYER_COLOR_COUNT;
}

}  // namespace rtype::client
