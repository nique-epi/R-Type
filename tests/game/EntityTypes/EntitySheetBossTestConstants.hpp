#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>
#include "TimeConstants.hpp"

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

/** @brief A hurt zone of one box and the sweeping beam of the Bydo Tyrant:
 * from (-124, 44), 1540 units long and 28 thick, turning from 35 degrees up
 * to 12 degrees down over the first 1200 ms of the strike of attack 15. */
constexpr std::string_view ZONE_WITH_A_SWEEPING_BEAM =
    "[[gameplay.hurt_zone.box]]\noffset_x = 0\noffset_y = 0\nwidth = 40\n"
    "height = 40\n\n"
    "[[gameplay.hurt_zone.beam]]\noffset_x = -124\noffset_y = 44\n"
    "length = 1540\nthickness = 28\nfrom_deg = -35\nto_deg = 12\n"
    "attack = 15\nphase = \"strike\"\nuntil_ms = 1200\n";
constexpr float SWEEPING_BEAM_OFFSET_X = -124.0F;
constexpr float SWEEPING_BEAM_OFFSET_Y = 44.0F;
constexpr float SWEEPING_BEAM_LENGTH = 1540.0F;
constexpr float SWEEPING_BEAM_THICKNESS = 28.0F;
constexpr float SWEEPING_BEAM_FROM_DEGREES = -35.0F;
constexpr float SWEEPING_BEAM_TO_DEGREES = 12.0F;
constexpr std::uint8_t SWEEPING_BEAM_ATTACK = 15;
constexpr engine::Duration SWEEPING_BEAM_UNTIL =
    std::chrono::milliseconds(1200);

/** @brief The same zone whose beam sweeps with no end, and where and why it
 * is refused. */
constexpr std::string_view ZONE_WITH_AN_ENDLESS_SWEEP =
    "[[gameplay.hurt_zone.box]]\noffset_x = 0\noffset_y = 0\nwidth = 40\n"
    "height = 40\n\n"
    "[[gameplay.hurt_zone.beam]]\noffset_x = -124\noffset_y = 44\n"
    "length = 1540\nthickness = 28\nfrom_deg = -35\nto_deg = 12\n"
    "attack = 15\nphase = \"strike\"\n";
constexpr std::string_view TO_DEGREES_DOTTED_KEY =
    "gameplay.hurt_zone.beam[0].to_deg";
constexpr std::string_view SWEEP_RULE_TEXT =
    "a beam that sweeps (to_deg other than from_deg) names until_ms";

}  // namespace rtype::game::testing
