#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>
#include "IInterfaceCanvas.hpp"
#include "PlayfieldMapping.hpp"
#include "RowScroll.hpp"

namespace rtype::client {

/** @brief Paints a mark of 5 pixels at its widest, SCROLL_MARK_ROWS tall,
 * its tip at @p tip and each row of pixels two wider than the one before:
 * pointing up when @p towards is 1, its rows going down, and pointing down
 * when it is -1. */
void paintScrollArrow(IInterfaceCanvas& canvas, sf::Vector2i tip, int towards);

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

/** @returns A mapping that reads a window pixel as @p mapping does, then
 * moves the point by @p offset, in pixels of the interface picture: the
 * place, in a picture painted moved by -offset, of what the pointer is over.
 */
[[nodiscard]] PlayfieldMapping shiftedMappingOf(
    sf::Vector2i offset, const PlayfieldMapping& mapping);

/**
 * @returns A mapping that reads a window pixel as @p mapping does, then moves
 * a point over the shown rows of @p scroll back to the place of its row; the
 * same mapping when the panel shows every row.
 */
[[nodiscard]] PlayfieldMapping scrolledMappingOf(
    const std::optional<RowScroll>& scroll, const PlayfieldMapping& mapping);

}  // namespace rtype::client
