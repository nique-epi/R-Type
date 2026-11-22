#pragma once

#include "LetterCounts.hpp"
#include "Translation.hpp"

namespace rtype::client {

/** @brief The words of the flight log: the left page, the right page and the
 * choices. YOU is LOCAL_PLAYER_TAG, P and a number playerNumberLabel(). */
constexpr Translation MISSION_LOG_TEXT{.english = "MISSION LOG",
                                       .french = "JOURNAL"};
constexpr Translation DEFEAT_STAMP_TEXT{.english = "GAME OVER",
                                        .french = "GAME OVER"};
constexpr Translation VICTORY_STAMP_TEXT{.english = "VICTORY",
                                         .french = "VICTOIRE"};
constexpr Translation DOWN_TEXT{.english = "ALL PILOTS DOWN",
                                .french = "PILOTES ABATTUS"};
constexpr Translation CLEARED_TEXT{.english = "MISSION CLEARED",
                                   .french = "MISSION RÉUSSIE"};

/** @brief The line under the stamp of a victory is as long as the one of a
 * defeat: RANKING_DELAY waits until the line is written whole, and the test
 * of the timeline checks that wait for DOWN_TEXT. */
static_assert(lettersIn(CLEARED_TEXT.english) == lettersIn(DOWN_TEXT.english) &&
                  lettersIn(CLEARED_TEXT.french) == lettersIn(DOWN_TEXT.french),
              "the right page waits for the line under the stamp either way");
constexpr Translation TEAM_SCORE_TEXT{.english = "TEAM SCORE",
                                      .french = "SCORE ÉQUIPE"};
constexpr Translation RANKING_TEXT{.english = "FINAL RANKING",
                                   .french = "CLASSEMENT"};
constexpr Translation PROMPT_TEXT{.english = "PRESS ENTER TO REPLAY",
                                  .french = "ENTRÉE POUR REJOUER"};
constexpr Translation QUIT_TEXT{.english = "QUIT", .french = "FIN"};

/** @brief The texts the timeline of the flight log waits for are timed on
 * their English, which is never shorter than their French: the French is typed
 * whole within the same time. */
static_assert(lettersIn(MISSION_LOG_TEXT.french) <=
                  lettersIn(MISSION_LOG_TEXT.english),
              "the French heading of the left page is typed in time");
static_assert(lettersIn(RANKING_TEXT.french) <= lettersIn(RANKING_TEXT.english),
              "the French heading of the right page is typed in time");
static_assert(lettersIn(PROMPT_TEXT.french) <= lettersIn(PROMPT_TEXT.english),
              "the French call to replay is typed in time");
static_assert(lettersIn(QUIT_TEXT.french) <= lettersIn(QUIT_TEXT.english),
              "the French label of QUIT fits its small button");

}  // namespace rtype::client
