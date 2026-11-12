#include "ShopLayout.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>
#include "ShopLayoutConstants.hpp"

namespace rtype::client {

sf::IntRect shopCellArea(std::size_t slot) {
  const auto column = static_cast<int>(slot % SHOP_LOCKER_COLUMN_COUNT);
  const auto row = static_cast<int>(slot / SHOP_LOCKER_COLUMN_COUNT);
  const int pitch = SHOP_CELL_SIZE + SHOP_CELL_GAP;
  return {SHOP_CELLS_TOP_LEFT + sf::Vector2i(column * pitch, row * pitch),
          {SHOP_CELL_SIZE, SHOP_CELL_SIZE}};
}

sf::IntRect shopStallArea(std::size_t hull) {
  const int pitch = SHOP_STALL_SIZE.x + SHOP_STALL_GAP;
  return {
      SHOP_STALLS_TOP_LEFT + sf::Vector2i(static_cast<int>(hull) * pitch, 0),
      SHOP_STALL_SIZE};
}

std::optional<std::size_t> shopCellAt(sf::Vector2i pixel) {
  for (std::size_t slot = 0; slot < SHOP_LOCKER_SLOT_COUNT; ++slot) {
    if (shopCellArea(slot).contains(pixel)) {
      return slot;
    }
  }
  return std::nullopt;
}

std::optional<std::size_t> shopStallAt(sf::Vector2i pixel,
                                       std::size_t hullCount) {
  for (std::size_t hull = 0; hull < hullCount; ++hull) {
    if (shopStallArea(hull).contains(pixel)) {
      return hull;
    }
  }
  return std::nullopt;
}

}  // namespace rtype::client
