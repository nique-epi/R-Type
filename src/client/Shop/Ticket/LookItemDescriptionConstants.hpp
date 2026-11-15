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
 * SHIP_LOOK_ITEM_NAMES: the accessories, then the hulls. Each line fits the
 * width of the ticket in the regular font, TICKET_TEXT_WIDTH.
 */
constexpr std::array<TicketDescription, identifiers::SHIP_LOOK_ITEM_COUNT>
    LOOK_ITEM_DESCRIPTIONS{{
        {"FOR THE GENTLEMAN", "WHO SHOOTS FIRST", "AND BOWS AFTER."},
        {"EVERY BOSS FIGHT", "IS A PARTY IF YOU", "BRING THE HAT."},
        {"REAL GOLD, MAYBE.", "RULE THE SECTOR", "FROM YOUR COCKPIT."},
        {"EXTRA LIFT. ZERO", "AERODYNAMICS.", "PURE STYLE."},
        {"THIS SECTOR AIN'T", "BIG ENOUGH FOR", "YOU AND THE BYDO."},
        {"HORNS SOLD", "SEPARATELY. NO,", "THEY ARE INCLUDED."},
        {"CASTS NO SPELLS.", "THE BYDO DO NOT", "KNOW THAT."},
        {"COOKS THE BYDO TO", "PERFECTION. WELL", "DONE, ALWAYS."},
        {"THE STARS ARE", "BRIGHT. YOUR", "FUTURE TOO."},
        {"GROWN IN ZERO G.", "TWENTY YEARS OF", "HARD WORK."},
        {"GENUINE MOSCOW FUR.", "WARM AT MINUS", "FORTY, COMRADE."},
        {"ARR, PILOT. THE", "VOID IS JUST A", "VERY DRY SEA."},
        {"HO HO HO. THE", "BYDO GET COAL", "THIS YEAR."},
        {"FOR PILOTS WHO", "NEVER LEAVE A", "WINGMAN BEHIND."},
        {"FOR PILOTS WHO", "LEAVE EVERY", "WINGMAN BEHIND."},
        {"SHADE FOR WHEN", "THE SUN IS A", "LITTLE TOO CLOSE."},
        {"STOLEN FROM A", "MOSCOW ROADWORK.", "DO NOT TELL."},
        {"IT IS A FEZ.", "FEZZES ARE COOL.", ""},
        {"DIPLOMA IN", "SHOOTING THINGS.", "WITH HONOURS."},
        {"PICKS UP RADIO", "MOSCOW AND THE", "BYDO GOSSIP."},
        {"HONOUR, STEEL", "AND A VERY TALL", "CREST."},
        {"PUNK IS NOT DEAD.", "IT FLIES A SHIP", "NOW."},
        {"THE BYDO LOOK", "EVEN UGLIER IN", "THREE DIMENSIONS."},
        {"ONE LENS, HALF", "THE PRICE OF", "GLASSES. SMART."},
        {"WISDOM GROWS", "SLOWLY. THIS ONE", "GREW FOR AGES."},
        {"TEETH PAINTED ON", "THE NOSE. THE", "BYDO HATE IT."},
        {"A GAGARIN CLASSIC.", "STILL SMELLS OF", "ROCKET FUEL."},
        {"A SHELL OF THE", "ENEMY. WEAR IT,", "CONFUSE THEM."},
        {"PURE GOLD PLATING.", "HEAVY, SHINY,", "WORTH IT."},
        {"THE RAREST HULL.", "EVEN THE BYDO", "BOW TO IT."},
    }};

}  // namespace rtype::client
