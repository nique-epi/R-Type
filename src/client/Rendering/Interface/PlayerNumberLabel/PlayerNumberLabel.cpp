#include "PlayerNumberLabel.hpp"
#include <string>
#include "InterfaceConstants.hpp"

namespace rtype::client {

std::string playerNumberLabel(int playerNumber) {
  return PLAYER_NUMBER_PREFIX + std::to_string(playerNumber);
}

}  // namespace rtype::client
