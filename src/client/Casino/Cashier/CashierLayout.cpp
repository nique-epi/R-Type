#include "CashierLayout.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>
#include "CashierConstants.hpp"
#include "CasinoSpriteConstants.hpp"

namespace rtype::client {

sf::Vector2i selectorChipTopLeft(std::size_t place, bool chosen) {
  return {
      CASHIER_FIRST_CHIP.x + (static_cast<int>(place) * CASHIER_CHIP_SPACING),
      CASHIER_FIRST_CHIP.y - (chosen ? CASHIER_CHOSEN_CHIP_LIFT : 0)};
}

std::optional<std::size_t> selectorChipAt(sf::Vector2i pixel) {
  for (std::size_t place = 0; place < CASHIER_STEPS.size(); ++place) {
    if (sf::IntRect(selectorChipTopLeft(place, false), CHIP_SELECTOR_FRAME_SIZE)
            .contains(pixel)) {
      return place;
    }
  }
  return std::nullopt;
}

}  // namespace rtype::client
