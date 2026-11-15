#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/**
 * @brief Where a bay of the waiting room lays its parts, from its top-left
 * corner: its size, the band of the ship color, how many times the ship is
 * enlarged and its top, the top of the nickname and how many lines it may
 * take, and the cell that says whether the pilot is ready.
 *
 * As the waiting room was drawn at the size the fonts are drawn at; above
 * it, the ship is drawn at its own size so that the nickname and the word of
 * the cell may take several lines, and the bay grows taller to hold them.
 */
struct HangarBayLayout {
  sf::Vector2i size;
  sf::IntRect colorBand;
  int shipScale{};
  int shipTop{};
  int nicknameTop{};
  int nicknameLines{};
  sf::IntRect stateCell;
};

}  // namespace rtype::client
