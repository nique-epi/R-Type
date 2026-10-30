#pragma once

#include <SFML/Graphics/Rect.hpp>

namespace rtype::client {

/** @brief Where the waiting room paints the parts its help explains, in
 * pixels of the interface picture. */
struct LobbyPartAreas {
  /** @brief The four bays, side by side. */
  sf::IntRect bays;
  /** @brief The bay of the local player; the four bays while the server
   * lists no player with its number. */
  sf::IntRect ownBay;
  sf::IntRect ready;
  /** @brief The status line, at least as wide as READY so that an empty line
   * still has a place to light. */
  sf::IntRect status;
  sf::IntRect back;
};

}  // namespace rtype::client
