#pragma once

#include <array>
#include <string_view>
#include "GameRuleConstants.hpp"
#include "Translation.hpp"

namespace rtype::client {

/** @brief Words of the line that names the rules of a game after its
 * difficulty: the separator, the lives for one and for several, the
 * friendly fire on and off, and the names of the difficulties a server
 * ships, in the order of DIFFICULTY_ORDER. A difficulty the client does not
 * know is written as the server names it, in capitals. */
constexpr const char* RULES_SEPARATOR = ", ";
constexpr std::string_view LIVES_MARK = "{LIVES}";
constexpr Translation ONE_LIFE_WORDS{.english = "{LIVES} LIFE",
                                     .french = "{LIVES} VIE"};
constexpr Translation LIVES_WORDS{.english = "{LIVES} LIVES",
                                  .french = "{LIVES} VIES"};
constexpr Translation FRIENDLY_FIRE_ON_WORDS{.english = "FRIENDLY FIRE ON",
                                             .french = "TIR AMI ACTIF"};
constexpr Translation FRIENDLY_FIRE_OFF_WORDS{.english = "FRIENDLY FIRE OFF",
                                              .french = "TIR AMI COUPÉ"};
constexpr std::array<Translation, identifiers::DIFFICULTY_ORDER.size()>
    DIFFICULTY_NAMES{Translation{.english = "EASY", .french = "FACILE"},
                     Translation{.english = "NORMAL", .french = "NORMAL"},
                     Translation{.english = "HARD", .french = "DIFFICILE"}};

}  // namespace rtype::client
