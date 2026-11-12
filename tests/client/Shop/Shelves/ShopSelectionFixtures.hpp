#pragma once

#include <SFML/Window/Keyboard.hpp>
#include "ShopSelection.hpp"

namespace rtype::client::testing {

/** @brief Moves @p selection with the arrow @p key, @p times times. */
void moveBy(ShopSelection& selection, sf::Keyboard::Key key, int times);

}  // namespace rtype::client::testing
