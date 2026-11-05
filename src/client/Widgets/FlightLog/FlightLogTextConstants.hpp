#pragma once

#include <string_view>

namespace rtype::client {

/** @brief The words of the flight log: the left page, the right page and the
 * choices. YOU is LOCAL_PLAYER_TAG, P and a number playerNumberLabel(). */
constexpr std::string_view MISSION_LOG_TEXT = "MISSION LOG";
constexpr std::string_view DEFEAT_STAMP_TEXT = "GAME OVER";
constexpr std::string_view VICTORY_STAMP_TEXT = "VICTORY";
constexpr std::string_view DOWN_TEXT = "ALL PILOTS DOWN";
constexpr std::string_view CLEARED_TEXT = "MISSION CLEARED";

/** @brief The line under the stamp of a victory is as long as the one of a
 * defeat, since the schedule of the book counts the letters of DOWN_TEXT. */
static_assert(CLEARED_TEXT.size() == DOWN_TEXT.size(),
              "the schedule of the book counts the same letters either way");
constexpr std::string_view TEAM_SCORE_TEXT = "TEAM SCORE";
constexpr std::string_view RANKING_TEXT = "FINAL RANKING";
constexpr std::string_view PROMPT_TEXT = "PRESS ENTER TO REPLAY";
constexpr std::string_view QUIT_TEXT = "QUIT";

}  // namespace rtype::client
