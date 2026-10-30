#pragma once

namespace rtype::game {

/**
 * @brief The directions a player holds for its ship. Holding two opposite
 * directions is holding neither.
 */
struct ShipDirections {
  bool up{false};
  bool down{false};
  bool left{false};
  bool right{false};
};

}  // namespace rtype::game
