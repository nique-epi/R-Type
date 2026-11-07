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

/** @brief A volley of every key: three Drifters in the strike of attack 3,
 * from (-40, -60), 120 units apart going down, spread over 20 degrees, 8
 * units out, the second one left out. */
constexpr std::string_view VOLLEY_OF_EVERY_KEY =
    "[[gameplay.volley]]\nattack = 3\nphase = \"strike\"\n"
    "body = \"drifter\"\ncount = 3\noffset_x = -40\noffset_y = -60\n"
    "step_x = 0\nstep_y = 120\nspread_deg = 20\ndistance = 8\n"
    "gap_from = 1\ngap_count = 1\n";
constexpr std::uint8_t VOLLEY_ATTACK = 3;
constexpr std::string_view VOLLEY_BODY = "drifter";
constexpr std::uint8_t VOLLEY_COUNT = 3;
constexpr float VOLLEY_OFFSET_X = -40.0F;
constexpr float VOLLEY_OFFSET_Y = -60.0F;
constexpr float VOLLEY_STEP_Y = 120.0F;
constexpr float VOLLEY_SPREAD_DEGREES = 20.0F;
constexpr float VOLLEY_DISTANCE = 8.0F;
constexpr std::uint8_t VOLLEY_GAP_FROM = 1;
constexpr std::uint8_t VOLLEY_GAP_COUNT = 1;

/** @brief A volley of its required keys only. */
constexpr std::string_view VOLLEY_OF_REQUIRED_KEYS =
    "[[gameplay.volley]]\nattack = 3\nphase = \"strike\"\n"
    "body = \"drifter\"\ncount = 3\noffset_x = -40\noffset_y = -60\n";

/** @brief A volley of two bodies whose gap takes both, and where and why it
 * is refused. */
constexpr std::string_view VOLLEY_WITH_NO_BODY_LEFT =
    "[[gameplay.volley]]\nattack = 3\nphase = \"strike\"\n"
    "body = \"drifter\"\ncount = 2\noffset_x = 0\noffset_y = 0\n"
    "gap_count = 2\n";
constexpr std::string_view GAP_COUNT_DOTTED_KEY =
    "gameplay.volley[0].gap_count";
constexpr std::string_view VOLLEY_GAP_RULE_TEXT =
    "a gap leaves at least one body of the volley";

/** @brief Where and why a volley on a missile is refused. */
constexpr std::string_view VOLLEY_DOTTED_KEY = "gameplay.volley";
constexpr std::string_view VOLLEY_SIDE_RULE_TEXT =
    "only a body of side enemy fires volleys";

/** @brief The table of a boss: it enrages below half its health into its
 * attack 9, and arrives in its attack 0; and where and why a boss table is
 * refused. */
constexpr std::string_view BOSS_TABLE =
    "[gameplay.boss]\nrage_below_percent = 50\nrage_attack = 9\n"
    "arrival_attack = 0\n";
constexpr int BOSS_RAGE_BELOW_PERCENT = 50;
constexpr std::uint8_t BOSS_TABLE_RAGE_ATTACK = 9;
constexpr std::uint8_t BOSS_TABLE_ARRIVAL_ATTACK = 0;
constexpr std::string_view BOSS_DOTTED_KEY = "gameplay.boss";
constexpr std::string_view BOSS_SIDE_RULE_TEXT =
    "only a body of side enemy is a boss";
constexpr std::string_view BOSS_HEALTH_RULE_TEXT = "a boss has health";

}  // namespace rtype::game::testing
