#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <optional>

namespace rtype::client {

/** @returns The cell @p slot of the locker, LOCKER_COLUMNS to a row. */
[[nodiscard]] sf::IntRect lockerCellArea(std::size_t slot);

/** @returns The item whose cell holds @p pixel; nothing between the cells
 * or over an empty one. */
[[nodiscard]] std::optional<std::size_t> lockerItemAt(sf::Vector2i pixel);

/** @returns The item the arrow @p key moves the choice to from @p from:
 * @p from itself against a side or an empty cell; nothing for another key. */
[[nodiscard]] std::optional<std::size_t> lockerItemTowards(
    std::size_t from, sf::Keyboard::Key key);

}  // namespace rtype::client
