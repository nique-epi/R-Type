#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>
#include "PlayfieldMapping.hpp"
#include "RowScroll.hpp"

namespace rtype::client {

/** @returns How far the shown rows of @p scroll move up from their place, as
 * an offset to paint them with: none when the first row shows. */
[[nodiscard]] sf::Vector2i rowOffsetOf(const RowScroll& scroll);

/** @returns Whether @p button is a row of @p scroll that does not show. */
[[nodiscard]] bool isHiddenRow(const RowScroll& scroll, std::size_t button);

/** @returns @p scroll moved the least so that @p button shows, when it is one
 * of its rows; unchanged otherwise. */
[[nodiscard]] RowScroll scrollShowing(RowScroll scroll, std::size_t button);

/** @returns @p scroll moved @p step rows down, up for a negative step, never
 * past its first or its last row. */
[[nodiscard]] RowScroll scrollMovedBy(RowScroll scroll, int step);

/**
 * @returns A mapping that reads a window pixel as @p mapping does, then moves
 * a point over the shown rows of @p scroll back to the place of its row; the
 * same mapping when the panel shows every row.
 */
[[nodiscard]] PlayfieldMapping scrolledMappingOf(
    const std::optional<RowScroll>& scroll, const PlayfieldMapping& mapping);

}  // namespace rtype::client
