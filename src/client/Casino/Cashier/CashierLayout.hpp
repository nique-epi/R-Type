#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>

namespace rtype::client {

/** @returns Where the chip of @p place of the selector lies, its top-left
 * corner: CASHIER_CHIP_SPACING apart from CASHIER_FIRST_CHIP, raised by
 * CASHIER_CHOSEN_CHIP_LIFT when @p chosen. */
[[nodiscard]] sf::Vector2i selectorChipTopLeft(std::size_t place, bool chosen);

/** @returns The place of the chip of the selector under @p pixel of the
 * interface, its frame at rest; nothing between the chips. */
[[nodiscard]] std::optional<std::size_t> selectorChipAt(sf::Vector2i pixel);

}  // namespace rtype::client
