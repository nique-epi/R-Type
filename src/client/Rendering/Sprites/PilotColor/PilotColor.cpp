#include "PilotColor.hpp"
#include "EntitySpriteConstants.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::client {

bool isPilotColor(int color) {
  return color >= 0 && color < PLAYER_COLOR_COUNT;
}

int pilotColorOfPlayerNumber(int playerNumber) {
  return playerNumber - network::FIRST_PLAYER_NUMBER;
}

int playerNumberOfPilotColor(int pilotColor) {
  return pilotColor + network::FIRST_PLAYER_NUMBER;
}

}  // namespace rtype::client
