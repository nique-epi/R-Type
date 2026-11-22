#pragma once

#include "Translation.hpp"

namespace rtype::client {

/** @brief Texts of the new game dialog: its heading, its rows but the
 * difficulty, whose label is DIFFICULTY_ROW_LABEL, and CREATE. */
constexpr Translation NEW_GAME_HEADING{.english = "NEW GAME",
                                       .french = "NOUVELLE PARTIE"};
constexpr Translation NEW_GAME_LIVES_LABEL{.english = "LIVES EACH",
                                           .french = "VIES CHACUN"};
constexpr Translation NEW_GAME_FRIENDLY_FIRE_LABEL{.english = "FRIENDLY FIRE",
                                                   .french = "TIR AMI"};
constexpr Translation CREATE_LABEL{.english = "CREATE", .french = "CRÉER"};

}  // namespace rtype::client
