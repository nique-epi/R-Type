#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::client::testing {

/** @brief A note the tests keep. */
constexpr std::string_view NOTE_LINE = "Leave a game from the options";

/** @brief A notes file whose line is a number, not a text. */
constexpr std::string_view NOTES_OF_A_NUMBER =
    R"({"version":"1.1.0","notes":[1]})";

/** @brief A note longer than the panel, and what is left of it. */
constexpr std::string_view LONG_NOTE =
    "A note that runs well past the right edge of the panel of WHAT'S NEW";
constexpr std::string_view CUT_NOTE =
    "A note that runs well past the right edge of the";

/** @brief A note holding a character the font does not draw, and what is
 * left of it. */
constexpr std::string_view ACCENTED_NOTE = "Caf\xc3\xa9 menu";
constexpr std::string_view UNACCENTED_NOTE = "Caf menu";

/** @brief Nine notes, one more than the panel holds. */
constexpr std::string_view NINE_NOTES = "1\n2\n3\n4\n5\n6\n7\n8\n9\n";
constexpr std::string_view LAST_NOTE_SHOWN = "8";
constexpr std::size_t NOTE_LINES_OF_THE_PANEL = 8;

}  // namespace rtype::client::testing
