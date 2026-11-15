#pragma once

#include <array>
#include <cstddef>
#include <string_view>
#include "ShipLookConstants.hpp"

namespace rtype::client {

/** @brief The most lines the description of a ware takes on its ticket. */
constexpr std::size_t TICKET_DESCRIPTION_LINE_COUNT = 3;

/** @brief The lines of the description of a ware, the unused ones empty. */
using TicketDescription =
    std::array<std::string_view, TICKET_DESCRIPTION_LINE_COUNT>;

/**
 * @brief What the ticket of each ware says of it, in the order of
 * SHIP_LOOK_ITEM_NAMES: the accessories, then the hulls. Each line is a
 * group of words that reads on its own, never a phrase cut in two, and fits
 * the width of the ticket in the regular font, TICKET_TEXT_WIDTH.
 */
constexpr std::array<TicketDescription, identifiers::SHIP_LOOK_ITEM_COUNT>
    LOOK_ITEM_DESCRIPTIONS{{
        {"SHOOT FIRST,", "BOW AFTER.", "A TRUE GENTLEMAN."},
        {"EVERY BOSS FIGHT", "IS A PARTY", "WITH THIS HAT."},
        {"REAL GOLD. MAYBE.", "RULE THE SECTOR", "FROM YOUR COCKPIT."},
        {"EXTRA LIFT.", "ZERO AERODYNAMICS.", "PURE STYLE."},
        {"THIS SECTOR", "AIN'T BIG ENOUGH", "FOR TWO, PARTNER."},
        {"HORNS INCLUDED.", "PILLAGING", "SOLD SEPARATELY."},
        {"CASTS NO SPELLS.", "BUT THE BYDO", "DON'T KNOW THAT."},
        {"COOKS EVERY BYDO", "TO PERFECTION.", "WELL DONE, ALWAYS."},
        {"THE STARS", "ARE BRIGHT.", "SO IS YOUR FUTURE."},
        {"GROWN IN ZERO G.", "TWENTY YEARS", "OF HARD WORK."},
        {"GENUINE MOSCOW FUR.", "WARM DOWN TO -40.", "COMRADE-APPROVED."},
        {"ARR, PILOT.", "THE VOID IS JUST", "A VERY DRY SEA."},
        {"HO HO HO.", "THIS YEAR, THE BYDO", "GET COAL."},
        {"FOR THE PILOTS", "WHO NEVER LEAVE", "A WINGMAN BEHIND."},
        {"FOR THE PILOTS", "WHO SHOOT FIRST", "AND NEVER REGRET."},
        {"BIG SHADE", "FOR WHEN A STAR", "GETS TOO CLOSE."},
        {"STOLEN FROM", "A MOSCOW ROADWORK.", "DO NOT TELL."},
        {"IT IS A FEZ.", "FEZZES ARE COOL.", ""},
        {"TOP OF THE CLASS", "IN SHOOTING THINGS.", "WITH HONOURS."},
        {"PICKS UP", "RADIO MOSCOW", "AND BYDO GOSSIP."},
        {"HONOUR. STEEL.", "A VERY TALL CREST.", ""},
        {"PUNK IS NOT DEAD.", "IT FLIES NOW.", ""},
        {"THE BYDO LOOK", "EVEN UGLIER IN 3D.", ""},
        {"ONE LENS.", "HALF THE PRICE.", "TWICE THE CLASS."},
        {"WISDOM TAKES TIME.", "THIS ONE TOOK", "TWENTY WINTERS."},
        {"TEETH PAINTED", "ON THE NOSE.", "THE BYDO HATE IT."},
        {"A GAGARIN CLASSIC.", "STILL SMELLS", "OF ROCKET FUEL."},
        {"AN ENEMY SHELL.", "WEAR IT.", "CONFUSE THEM."},
        {"PURE GOLD PLATING.", "HEAVY. SHINY.", "WORTH IT."},
        {"THE RAREST HULL.", "EVEN THE BYDO", "BOW TO IT."},
    }};

}  // namespace rtype::client
