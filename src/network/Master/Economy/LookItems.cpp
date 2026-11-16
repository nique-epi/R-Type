#include "LookItems.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>
#include "ShipLookConstants.hpp"
#include "ShipLookPriceConstants.hpp"

namespace rtype::network {

std::uint32_t lookItemPrice(std::size_t item) {
  return SHIP_LOOK_ITEM_PRICES.at(item);
}

std::optional<std::size_t> lookItemOfName(std::string_view name) {
  for (std::size_t item = 0; item < identifiers::SHIP_LOOK_ITEM_COUNT; ++item) {
    if (identifiers::SHIP_LOOK_ITEM_NAMES.at(item) == name) {
      return item;
    }
  }
  return std::nullopt;
}

}  // namespace rtype::network
