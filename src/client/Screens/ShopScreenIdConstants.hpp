#pragma once

#include <SFML/Window/Keyboard.hpp>
#include "ScreenId.hpp"

namespace rtype::client {

/** @brief The shop, pushed over the server map: the looks and the hulls the
 * player buys with kopecks. */
constexpr ScreenId SHOP_SCREEN{"shop"};

/** @brief The name the map shows, the key that opens the shop from the map,
 * and what its keycap says. */
constexpr const char* SHOP_LABEL = "SHOP";
constexpr sf::Keyboard::Key SHOP_KEY = sf::Keyboard::Key::S;
constexpr const char* SHOP_KEY_NAME = "S";

}  // namespace rtype::client
