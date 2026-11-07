#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::game::testing {

/** @brief Two sheets, an enemy and a player, given out of the order of their
 * names. */
constexpr std::string_view WEAVER_TYPE = "weaver";
constexpr std::string_view WEAVER_TEXT =
    "[gameplay]\nside = \"enemy\"\n[appearance]\n";
constexpr std::string_view SHIP_TYPE = "ship";
constexpr std::string_view SHIP_TEXT =
    "[gameplay]\nside = \"player\"\n[appearance]\n";
constexpr std::size_t TWO_TYPES = 2;
constexpr std::size_t ONE_TYPE = 1;

/** @brief A sheet whose pilot is a behaviour only a plugin of its server
 * knows, and one without side. */
constexpr std::string_view LANCER_TYPE = "lancer";
constexpr std::string_view SHEET_OF_A_PLUGIN_PILOT =
    "[gameplay]\nside = \"enemy\"\npilot = \"lancer_charge\"\n"
    "[gameplay.behaviour]\nwindup_ms = 400\n[appearance]\n";
constexpr std::string_view SHEET_WITHOUT_SIDE =
    "[gameplay]\npilot = \"drifter\"\n[appearance]\n";

}  // namespace rtype::game::testing
