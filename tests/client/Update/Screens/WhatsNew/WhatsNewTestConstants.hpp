#pragma once

#include <string_view>

namespace rtype::client::testing {

/** @brief The notes WHAT'S NEW is built with. */
constexpr std::string_view NOTES_VERSION_TEXT = "1.1.0";
constexpr std::string_view FIRST_NOTE = "Leave a game";
constexpr std::string_view SECOND_NOTE = "Serve https";
constexpr std::string_view WHATS_NEW_HEADING_OF_THE_NOTES =
    "WHAT'S NEW IN VER 1.1.0";

}  // namespace rtype::client::testing
