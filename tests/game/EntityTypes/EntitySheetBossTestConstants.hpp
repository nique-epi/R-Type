#pragma once

#include <string_view>

namespace rtype::game::testing {

/** @brief A hitbox laid away from the center of its body, as the Bydo Tyrant
 * lays it on its eye: 72 by 72, its center 76 units left of the Position of
 * the body and 52 above it. */
constexpr std::string_view MOVED_HITBOX =
    "[gameplay.hitbox]\nwidth = 72\nheight = 72\noffset_x = -76\n"
    "offset_y = -52\n";
constexpr float MOVED_HITBOX_SIDE = 72.0F;
constexpr float MOVED_HITBOX_OFFSET_X = -76.0F;
constexpr float MOVED_HITBOX_OFFSET_Y = -52.0F;

/** @brief The box of a ship, 128 by 64, moved 8 units to the right of its
 * Position. */
constexpr std::string_view MOVED_SHIP_HITBOX =
    "[gameplay.hitbox]\nwidth = 128\nheight = 64\noffset_x = 8\n";

/** @brief The dotted key and the text a refused ship hitbox is told by. */
constexpr std::string_view HITBOX_DOTTED_KEY = "gameplay.hitbox";
constexpr std::string_view SHIP_HITBOX_RULE_TEXT =
    "a ship collides with the box shipAfter() keeps on screen";

}  // namespace rtype::game::testing
