#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::game::testing {

/** @brief The name every sheet of these tests is read under. */
constexpr std::string_view SHEET_NAME = "drifter";

/** @brief A sheet whose `[appearance]` names no effect and no look. */
constexpr std::string_view SHEET_WITHOUT_VISUALS =
    "[gameplay]\nside = \"enemy\"\n[appearance]\n";

/** @brief A sheet naming two effects and two looks, each given out of the
 * order of the names. */
constexpr std::string_view SHEET_WITH_VISUALS =
    "[gameplay]\nside = \"enemy\"\n[appearance]\n"
    "[appearance.effects.spark]\n[appearance.effects.burst]\n"
    "[appearance.looks.cracked]\n[appearance.looks.angry]\n";
constexpr std::string_view FIRST_EFFECT = "burst";
constexpr std::string_view SECOND_EFFECT = "spark";
constexpr std::string_view FIRST_LOOK = "angry";
constexpr std::string_view SECOND_LOOK = "cracked";
constexpr std::size_t TWO_NAMES = 2;
constexpr std::string_view UNKNOWN_NAME = "smoke";
constexpr std::string_view BASE_LOOK{};

/** @brief The numbers the looks of SHEET_WITH_VISUALS take: their places in
 * the order of the names, from 1, and 0 for the base loop. */
constexpr std::uint8_t BASE_LOOK_NUMBER = 0;
constexpr std::uint8_t FIRST_LOOK_NUMBER = 1;
constexpr std::uint8_t SECOND_LOOK_NUMBER = 2;

/** @brief Sheets naming the most effects a sheet may name, 16, and one more. */
constexpr std::string_view SHEET_WITH_THE_MOST_EFFECTS =
    "[gameplay]\nside = \"enemy\"\n[appearance]\n[appearance.effects]\n"
    "e00 = {}\ne01 = {}\ne02 = {}\ne03 = {}\ne04 = {}\ne05 = {}\ne06 = {}\ne07 "
    "= {}\ne08 = {}\ne09 = {}\ne10 = {}\ne11 = {}\ne12 = {}\ne13 = {}\ne14 = "
    "{}\ne15 = {}\n";
constexpr std::string_view SHEET_WITH_ONE_EFFECT_TOO_MANY =
    "[gameplay]\nside = \"enemy\"\n[appearance]\n[appearance.effects]\n"
    "e00 = {}\ne01 = {}\ne02 = {}\ne03 = {}\ne04 = {}\ne05 = {}\ne06 = {}\ne07 "
    "= {}\ne08 = {}\ne09 = {}\ne10 = {}\ne11 = {}\ne12 = {}\ne13 = {}\ne14 = "
    "{}\ne15 = {}\ne16 = {}\n";
constexpr std::size_t MOST_EFFECTS_OF_A_SHEET = 16;

/** @brief Sheets naming a look with 24 letters, the longest name, with 25,
 * with a capital letter, and naming as an effect something else than a
 * table. */
constexpr std::string_view LONGEST_LOOK = "abcdefghijklmnopqrstuvwx";
constexpr std::string_view SHEET_WITH_THE_LONGEST_LOOK =
    "[gameplay]\nside = \"enemy\"\n[appearance]\n"
    "[appearance.looks.abcdefghijklmnopqrstuvwx]\n";
constexpr std::string_view SHEET_WITH_A_TOO_LONG_LOOK =
    "[gameplay]\nside = \"enemy\"\n[appearance]\n"
    "[appearance.looks.abcdefghijklmnopqrstuvwxy]\n";
constexpr std::string_view SHEET_WITH_A_CAPITAL_IN_AN_EFFECT =
    "[gameplay]\nside = \"enemy\"\n[appearance]\n"
    "[appearance.effects.Spark]\n";
constexpr std::string_view SHEET_WITH_AN_EFFECT_THAT_IS_NO_TABLE =
    "[gameplay]\nside = \"enemy\"\n[appearance]\n"
    "[appearance.effects]\nspark = 3\n";

}  // namespace rtype::game::testing
