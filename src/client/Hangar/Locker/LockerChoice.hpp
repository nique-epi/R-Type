#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <optional>
#include "RowWindow.hpp"

namespace rtype::client {

/** @returns The cell at place @p slot among the LOCKER_SLOT_COUNT cells
 * shown, LOCKER_COLUMNS to a row. */
[[nodiscard]] sf::IntRect lockerCellArea(std::size_t slot);

/** @returns The item whose cell holds @p pixel while the locker shows its
 * rows from @p firstRow; nothing between the cells or over an empty one. */
[[nodiscard]] std::optional<std::size_t> lockerItemAt(sf::Vector2i pixel,
                                                      std::size_t firstRow);

/** @returns The item the arrow @p key moves the choice to from @p from, among
 * every item, shown or not: @p from itself against a side or an empty cell;
 * nothing for another key. */
[[nodiscard]] std::optional<std::size_t> lockerItemTowards(
    std::size_t from, sf::Keyboard::Key key);

/** @returns The row of the locker @p item sits in. */
[[nodiscard]] std::size_t lockerRowOf(std::size_t item);

/** @returns @p item when @p rows shows it, else the item of its column in
 * the nearest row shown, the last item when that cell is empty. */
[[nodiscard]] std::size_t lockerItemShownIn(std::size_t item, RowWindow rows);

}  // namespace rtype::client
