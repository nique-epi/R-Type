#pragma once

#include <SFML/Graphics/Rect.hpp>

namespace rtype::client {

/** @brief What the front of the casino shows under its lights and its bars:
 * the picture of the front and the butler. */
struct EntranceView {
  /** @brief The picture of the front, its doors closed or open. */
  const char* front{};
  /** @brief The frame of the butler on his sheet. */
  sf::IntRect butler;
};

}  // namespace rtype::client
