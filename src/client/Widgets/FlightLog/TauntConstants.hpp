#pragma once

#include <array>
#include <string_view>
#include "Taunt.hpp"
#include "TauntCondition.hpp"

namespace rtype::client {

constexpr char TAUNT_LINE_BREAK = '\n';
constexpr std::string_view BEST_PILOT_MARK = "{BEST}";
constexpr std::string_view LAST_PILOT_MARK = "{LAST}";
constexpr std::string_view ZERO_PILOT_MARK = "{ZERO}";

/** @brief The taunts the cover picks from, one per game: the squadron lost,
 * and the book says so. */
constexpr std::array TAUNTS{
    Taunt{.text = "THE BYDO SEND\nTHEIR REGARDS",
          .condition = TauntCondition::Always},
    Taunt{.text = "NOT YOUR\nFINEST HOUR", .condition = TauntCondition::Always},
    Taunt{.text = "SPACE IS HARD.\nYOU MADE IT\nHARDER.",
          .condition = TauntCondition::Always},
    Taunt{.text = "WELL...\nYOU TRIED.", .condition = TauntCondition::Always},
    Taunt{.text = "SKILL ISSUE\nDETECTED", .condition = TauntCondition::Always},
    Taunt{.text = "THE BYDO SAY:\nGG EZ", .condition = TauntCondition::Always},
    Taunt{.text = "MISSION FAILED\nSUCCESSFULLY",
          .condition = TauntCondition::Always},
    Taunt{.text = "EARTH WAS NICE\nWHILE IT LASTED",
          .condition = TauntCondition::Always},
    Taunt{.text = "INSURANCE DOES\nNOT COVER THIS",
          .condition = TauntCondition::Always},
    Taunt{.text = "HAVE YOU TRIED\nNOT GETTING HIT?",
          .condition = TauntCondition::Always},
    Taunt{.text = "BOLD STRATEGY:\nEXPLODING",
          .condition = TauntCondition::Always},
    Taunt{.text = "WHO FORGOT\nTHE FORCE?",
          .condition = TauntCondition::Always},
    Taunt{.text = "THE BYDO WOULD\nLIKE A REMATCH",
          .condition = TauntCondition::Always},
    Taunt{.text = "PRACTICE MAKES\nYOU LESS DEAD",
          .condition = TauntCondition::Always},
    Taunt{.text = "BETTER LUCK\nNEXT LIFE",
          .condition = TauntCondition::Always},
    Taunt{.text = "NOBODY READ\nTHE MANUAL",
          .condition = TauntCondition::Always},
    Taunt{.text = "DEFEAT, BUT\nWITH STYLE",
          .condition = TauntCondition::Always},
    Taunt{.text = "THE CAT COULD\nHAVE DONE BETTER",
          .condition = TauntCondition::Always},
    Taunt{.text = "A TEAM EFFORT.\nSADLY.",
          .condition = TauntCondition::SeveralPilots},
    Taunt{.text = "{BEST}\nCARRIED THE TEAM",
          .condition = TauntCondition::BestAheadOfLast},
    Taunt{.text = "{LAST}\nWAS ALSO THERE",
          .condition = TauntCondition::BestAheadOfLast},
    Taunt{.text = "{ZERO}\nSCORED ZERO.\nIMPRESSIVE.",
          .condition = TauntCondition::ZeroScore},
    Taunt{.text = "SOLO AND\nSTILL OUTNUMBERED",
          .condition = TauntCondition::Solo}};

}  // namespace rtype::client
