#pragma once

#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/**
 * @brief Where the ranking lays its parts, from the top-left corner of its
 * page: the rule under the heading and how wide it and the separators are,
 * the top of the first row and from one row to the next; then, from the
 * top-left corner of a row, the marker, YOU, where the dotted leader ends
 * and on which row, where the score ends, the row of the separator, and the
 * medal.
 */
struct RankingLayout {
  sf::Vector2i ruleTopLeft;
  int lineWidth{};
  int firstRowTop{};
  int rowPitch{};
  sf::Vector2i markerOffset;
  sf::Vector2i youTagOffset;
  int leaderRight{};
  int leaderRow{};
  sf::Vector2i scoreEnd;
  int separatorRow{};
  sf::Vector2i medalOffset;
};

}  // namespace rtype::client
