#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>
#include <span>
#include "MapDirection.hpp"

namespace rtype::client {

/**
 * @returns The index of the star of @p stars nearest to the star @p from,
 * among those that lie towards @p direction, 45 degrees at most each side of
 * it; the lower index on a tie. Nothing when no star lies that way, or when
 * @p from names no star.
 */
[[nodiscard]] std::optional<std::size_t> neighbourOf(
    std::span<const sf::Vector2i> stars, std::size_t from,
    MapDirection direction);

/**
 * @returns The index of the star of @p stars nearest to @p pointer, among
 * those at most STAR_PICK_REACH pixels from it across and down; the lower
 * index on a tie. Nothing when no star is that close.
 */
[[nodiscard]] std::optional<std::size_t> starUnder(
    std::span<const sf::Vector2i> stars, sf::Vector2i pointer);

}  // namespace rtype::client
