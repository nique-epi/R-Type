#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::game::testing {

/** @brief Two sheets, an enemy and a player, given out of the order of their
 * names. */
constexpr std::string_view WEAVER_TYPE = "weaver";
constexpr std::string_view WEAVER_TEXT =
    "[gameplay]\nside = \"enemy\"\n"
    "[gameplay.hitbox]\nwidth = 96\nheight = 96\n"
    "[appearance]\n";
constexpr std::string_view SHIP_TYPE = "ship";
constexpr std::string_view SHIP_TEXT =
    "[gameplay]\nside = \"player\"\n"
    "[gameplay.hitbox]\nwidth = 128\nheight = 64\n"
    "[appearance]\n";
constexpr std::size_t TWO_TYPES = 2;
constexpr std::size_t ONE_TYPE = 1;

/** @brief A sheet whose pilot is a behaviour only a plugin of its server
 * knows, with a box of LANCER_BOX_WIDTH by LANCER_BOX_HEIGHT, one without side
 * and one without box. */
constexpr std::string_view LANCER_TYPE = "lancer";
constexpr std::string_view SHEET_OF_A_PLUGIN_PILOT =
    "[gameplay]\nside = \"enemy\"\npilot = \"lancer_charge\"\n"
    "[gameplay.hitbox]\nwidth = 64\nheight = 32\n"
    "[gameplay.behaviour]\nwindup_ms = 400\n[appearance]\n";
constexpr float LANCER_BOX_WIDTH = 64.0F;
constexpr float LANCER_BOX_HEIGHT = 32.0F;
constexpr std::string_view SHEET_WITHOUT_SIDE =
    "[gameplay]\npilot = \"drifter\"\n[appearance]\n";
constexpr std::string_view SHEET_WITHOUT_BOX =
    "[gameplay]\nside = \"enemy\"\n[appearance]\n";

}  // namespace rtype::game::testing
