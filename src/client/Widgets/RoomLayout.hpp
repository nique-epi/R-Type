#pragma once

#include <optional>

namespace rtype::client {

/**
 * @brief Where a room lays its parts, in pixels of the interface picture,
 * the room unscrolled: the top and height of its bars, the top of the status
 * line and how wide it may run, the height of a chip and the top of the two
 * rows of chips, and how tall the whole room is.
 *
 * As the room was drawn at the size the fonts are drawn at, one row of chips
 * against the bottom of the safe margin, both tops equal, and the room as
 * tall as the picture. Above it, a status line of ROOM_STATUS_LINES lines,
 * two rows of chips under it, and a room taller than the picture, which then
 * scrolls.
 */
struct RoomLayout {
  int barsTop{};
  int barHeight{};
  int statusTop{};
  std::optional<int> statusWidth;
  int chipHeight{};
  int upperChipsTop{};
  int lowerChipsTop{};
  int height{};
};

}  // namespace rtype::client
