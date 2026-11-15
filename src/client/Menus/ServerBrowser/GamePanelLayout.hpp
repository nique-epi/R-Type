#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <cstddef>
#include <optional>

namespace rtype::client {

/**
 * @brief Where the panel of the games lays its parts, from the top-left
 * corner of its area: the area, its rule, the top of its first row and from
 * one row to the next, how many rows show, how tall the highlight of a row is,
 * whether a row takes two lines and where its second line stands, and the top
 * and width of the line about the chosen game, none for one line.
 */
struct GamePanelLayout {
  sf::IntRect area;
  int ruleTop{};
  int firstRow{};
  int rowPitch{};
  std::size_t rowsShown{};
  int highlightHeight{};
  bool stacked{};
  int secondLine{};
  int descriptionTop{};
  std::optional<int> descriptionWidth;
};

}  // namespace rtype::client
