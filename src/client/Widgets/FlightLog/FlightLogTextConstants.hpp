#pragma once

#include <string_view>

namespace rtype::client {

/** @brief The words of the flight log: the left page, the right page and the
 * choices. YOU is LOCAL_PLAYER_TAG, P and a number playerNumberLabel(). */
constexpr std::string_view MISSION_LOG_TEXT = "MISSION LOG";
constexpr std::string_view STAMP_TEXT = "GAME OVER";
constexpr std::string_view DOWN_TEXT = "ALL PILOTS DOWN";
constexpr std::string_view TEAM_SCORE_TEXT = "TEAM SCORE";
constexpr std::string_view RANKING_TEXT = "FINAL RANKING";
constexpr std::string_view PROMPT_TEXT = "PRESS ENTER TO REPLAY";
constexpr std::string_view QUIT_TEXT = "QUIT";

}  // namespace rtype::client
