#pragma once

#include <optional>
#include "ActionKey.hpp"
#include "HurtWindow.hpp"

namespace rtype::game {

/**
 * @brief A rectangle that hurts a ship, in logical units, centered at
 * (offsetX, offsetY) from the Position of the enemy (y grows downwards).
 * `activeIn` names the attack and the phase it hurts in; empty, it hurts all
 * the time. `window` narrows when it hurts within that phase.
 */
struct HurtBox {
  float offsetX{0.0F};
  float offsetY{0.0F};
  float width{0.0F};
  float height{0.0F};
  std::optional<ActionKey> activeIn;
  HurtWindow window{};

  bool operator==(const HurtBox&) const = default;
};

}  // namespace rtype::game
