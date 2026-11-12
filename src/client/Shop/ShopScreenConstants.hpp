#pragma once

#include <string_view>
#include "SoundConstants.hpp"

namespace rtype::client {

/** @brief The sound of a purchase: the metal impact of the logo, the
 * nearest to a coin set down among the sounds of the client. */
constexpr const char* PURCHASE_SOUND_ID = LOGO_IMPACT_SOUND_ID;

/** @brief The status line of the shop: a ware and its price, a ware the
 * player owns, a purchase, and why one was refused. */
constexpr std::string_view BUY_HINT_START = " - ENTER: BUY FOR ";
constexpr std::string_view BUY_HINT_END = " KOPECKS";
constexpr std::string_view OWNED_HINT = " - ALREADY YOURS";
constexpr std::string_view BOUGHT_STATUS_START = "BOUGHT: ";
constexpr std::string_view ALREADY_YOURS_STATUS = "ALREADY YOURS";
constexpr std::string_view NOT_FOR_SALE_STATUS = "NOT FOR SALE";
constexpr std::string_view NOT_ENOUGH_STATUS_START = "NOT ENOUGH KOPECKS: ";
constexpr std::string_view NOT_ENOUGH_STATUS_END = " MISSING";

}  // namespace rtype::client
