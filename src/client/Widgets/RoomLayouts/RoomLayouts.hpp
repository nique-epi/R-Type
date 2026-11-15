#pragma once

#include "RoomLayout.hpp"
#include "TextMetrics.hpp"

namespace rtype::client {

/** @returns The layout of a room whose illustration is @p illustrationHeight
 * tall, 0 for none, at the text size of @p metrics: its bars
 * SPACE_ABOVE_ROOM_BARS under the illustration, the status line BUTTON_GAP
 * under them, then its chips. */
[[nodiscard]] RoomLayout roomLayoutOf(int illustrationHeight,
                                      TextMetrics metrics);

}  // namespace rtype::client
