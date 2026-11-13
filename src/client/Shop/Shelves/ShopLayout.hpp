#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>

namespace rtype::client {

/** @returns The cell at place @p slot among the SHOP_LOCKER_SLOT_COUNT cells
 * the locker of the shop shows, SHOP_LOCKER_COLUMNS to a row. */
[[nodiscard]] sf::IntRect shopCellArea(std::size_t slot);

/** @returns The stall of the hull at place @p hull of the glass case. */
[[nodiscard]] sf::IntRect shopStallArea(std::size_t hull);

/** @returns The cell shown that holds @p pixel; nothing between the cells. */
[[nodiscard]] std::optional<std::size_t> shopCellAt(sf::Vector2i pixel);

/** @returns The stall, among the first @p hullCount, that holds @p pixel;
 * nothing elsewhere. */
[[nodiscard]] std::optional<std::size_t> shopStallAt(sf::Vector2i pixel,
                                                     std::size_t hullCount);

}  // namespace rtype::client
