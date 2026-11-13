#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>

namespace rtype::client {

/** @brief The locker of the shop, as the picture of the booth draws it:
 * SHOP_LOCKER_COLUMNS cells a row, SHOP_LOCKER_ROWS rows shown, each cell
 * SHOP_CELL_SIZE large, SHOP_CELL_GAP apart, from SHOP_CELLS_TOP_LEFT. */
constexpr sf::Vector2i SHOP_CELLS_TOP_LEFT{16, 72};
constexpr int SHOP_CELL_SIZE = 32;
constexpr int SHOP_CELL_GAP = 4;
constexpr int SHOP_LOCKER_COLUMNS = 3;
constexpr int SHOP_LOCKER_ROWS = 4;
constexpr std::size_t SHOP_LOCKER_COLUMN_COUNT =
    static_cast<std::size_t>(SHOP_LOCKER_COLUMNS);
constexpr std::size_t SHOP_LOCKER_SHOWN_ROW_COUNT =
    static_cast<std::size_t>(SHOP_LOCKER_ROWS);
constexpr std::size_t SHOP_LOCKER_SLOT_COUNT =
    SHOP_LOCKER_COLUMN_COUNT * SHOP_LOCKER_SHOWN_ROW_COUNT;

/** @brief The scroll mark of that locker: a track right of its cells, as
 * tall as the rows shown. */
constexpr sf::IntRect SHOP_SCROLL_TRACK{
    {SHOP_CELLS_TOP_LEFT.x + (SHOP_LOCKER_COLUMNS * SHOP_CELL_SIZE) +
         ((SHOP_LOCKER_COLUMNS - 1) * SHOP_CELL_GAP) + 2,
     SHOP_CELLS_TOP_LEFT.y},
    {2, (SHOP_LOCKER_ROWS * SHOP_CELL_SIZE) +
            ((SHOP_LOCKER_ROWS - 1) * SHOP_CELL_GAP)}};

/** @brief The glass case of the counter: a stall per hull on its velvet,
 * which the picture draws from (158, 204), 164 by 40. Five stalls
 * SHOP_STALL_SIZE large, SHOP_STALL_GAP apart, fill its width: a ship, then
 * its price tag under it, never past the velvet. */
constexpr sf::Vector2i SHOP_STALLS_TOP_LEFT{158, 206};
constexpr sf::Vector2i SHOP_STALL_SIZE{32, 36};
constexpr int SHOP_STALL_GAP = 1;

}  // namespace rtype::client
