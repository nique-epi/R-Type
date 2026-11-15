#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include "TextStyle.hpp"

namespace rtype::client {

/**
 * @brief Where the mission log lays its parts, from the top-left corner of
 * its page: how wide the page is, the box of the stamp, the top of the line
 * under it and of the label of the team score, where the team score ends,
 * the top of the polaroids, and where and how a polaroid writes its caption,
 * from its own top-left corner.
 */
struct MissionLogLayout {
  int width{};
  sf::IntRect stampBox;
  int summaryTop{};
  int teamLabelTop{};
  sf::Vector2i teamScoreEnd;
  int polaroidTop{};
  sf::Vector2i caption;
  TextAlignment captionAlignment = TextAlignment::Left;
};

}  // namespace rtype::client
