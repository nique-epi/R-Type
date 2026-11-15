#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include "ScreenId.hpp"
#include "TabRailTab.hpp"

namespace rtype::client {

/** @brief One tab over the server map: how the rail shows it, the key that
 * opens it, and the screen it pushes over the map; SECTORS, the map itself,
 * has neither, and a locked tab no screen. */
struct ServerMapTab {
  TabRailTab tab;
  std::optional<sf::Keyboard::Key> key;
  std::optional<ScreenId> screen;
};

}  // namespace rtype::client
