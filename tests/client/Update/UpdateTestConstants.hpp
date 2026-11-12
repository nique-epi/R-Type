#pragma once

#include <string_view>

namespace rtype::client::testing {

/** @brief The version the notes of the tests describe. */
constexpr std::string_view NOTES_VERSION = "1.1.0";

/** @brief The notes of a tag, in Markdown, and the lines the panel shows of
 * them. */
constexpr std::string_view TAG_NOTES =
    "## Changes\n\n- Leave a game from the options\n* Faster menus  \n";
constexpr std::string_view TAG_FIRST_NOTE = "## Changes";
constexpr std::string_view TAG_SECOND_NOTE = "Leave a game from the options";
constexpr std::string_view TAG_THIRD_NOTE = "Faster menus";

}  // namespace rtype::client::testing
