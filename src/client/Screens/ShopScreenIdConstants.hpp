#pragma once

#include <SFML/Window/Keyboard.hpp>
#include "ScreenId.hpp"

namespace rtype::client {

/** @brief The shop, pushed over the server map by its SHOP tab: the looks
 * and the hulls the player buys with kopecks. */
constexpr ScreenId SHOP_SCREEN{"shop"};

/** @brief The name of the tab of the server map that opens the shop, and
 * the key that opens it from the map. */
constexpr const char* SHOP_LABEL = "SHOP";
constexpr sf::Keyboard::Key SHOP_KEY = sf::Keyboard::Key::S;

}  // namespace rtype::client
