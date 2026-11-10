#pragma once

#include <chrono>
#include <string_view>

namespace rtype::client::testing {

/** @brief One frame of the panel, at 60 frames a second. */
constexpr std::chrono::milliseconds UPDATE_FRAME{16};

/** @brief The line of the versions the panel is built with. */
constexpr std::string_view VERSION_LINE = "VER 1.0.0 > VER 1.1.0";

/** @brief A package 42 % received, then received whole. */
constexpr int PERCENT_RECEIVED = 42;
constexpr int WHOLE_PACKAGE = 100;
constexpr std::string_view DOWNLOADING_42_PERCENT = "DOWNLOADING 42%";

/** @brief Just short of the 3 s UPDATE FAILED shows, then enough more to
 * pass them; 0.9 s, short of the 1 s RESTARTING shows, then past it. */
constexpr std::chrono::milliseconds SHORT_OF_THE_FAILURE{2900};
constexpr std::chrono::milliseconds PAST_THE_MESSAGE{200};
constexpr std::chrono::milliseconds SHORT_OF_THE_RESTART{900};
constexpr std::chrono::milliseconds A_FEW_FRAMES{100};

/** @brief The notes WHAT'S NEW is built with. */
constexpr std::string_view NOTES_VERSION_TEXT = "1.1.0";
constexpr std::string_view FIRST_NOTE = "Leave a game";
constexpr std::string_view SECOND_NOTE = "Serve https";
constexpr std::string_view WHATS_NEW_HEADING_OF_THE_NOTES =
    "WHAT'S NEW IN VER 1.1.0";

}  // namespace rtype::client::testing
