#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/** @returns How wide the tag of @p price is: 23 pixels for four digits, 27
 * for five. */
[[nodiscard]] int priceTagWidth(std::uint32_t price);

/** @brief Paints the tag of @p price, a coin then its digits, ending at
 * @p topRight: its right side is one column left of it. */
void paintPriceTag(IInterfaceCanvas& canvas, sf::Vector2i topRight,
                   std::uint32_t price);

}  // namespace rtype::client
