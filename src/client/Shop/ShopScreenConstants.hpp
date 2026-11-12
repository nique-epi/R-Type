#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <string_view>
#include "SoundConstants.hpp"

namespace rtype::client {

/** @brief The sound of a purchase: the metal impact of the logo, the
 * nearest to a coin set down among the sounds of the client. */
constexpr const char* PURCHASE_SOUND_ID = LOGO_IMPACT_SOUND_ID;

/** @brief The radio of the counter, as the picture of the booth draws it: a
 * click on it, or SHOP_RADIO_KEY, changes the music between the music of
 * the shop and the music of the radio. */
constexpr sf::IntRect SHOP_RADIO_AREA{{280, 161}, {42, 21}};
constexpr sf::Keyboard::Key SHOP_RADIO_KEY = sf::Keyboard::Key::R;

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
