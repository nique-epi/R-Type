#pragma once

#include <cstddef>
#include <string_view>

/** @brief Two content folders: the first holds two scripts, given out of the
 * order of their names, and a file that is not a script; the second holds
 * no script folder. */
constexpr std::string_view FIRST_CONTENT_FOLDER = "crimson";
constexpr std::string_view SECOND_CONTENT_FOLDER = "plain";
constexpr std::string_view LATER_SCRIPT = "crimson/server/zigzag.luau";
constexpr std::string_view EARLIER_SCRIPT = "crimson/server/charge.luau";
constexpr std::string_view NOT_A_SCRIPT = "crimson/server/notes.txt";
constexpr std::string_view LATER_SCRIPT_TEXT = "-- zigzag";
constexpr std::string_view EARLIER_SCRIPT_TEXT = "-- charge";
constexpr std::size_t SCRIPTS_OF_THE_FOLDERS = 2;
