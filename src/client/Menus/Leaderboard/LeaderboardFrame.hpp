#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include "ItemListLayout.hpp"
#include "TextMetrics.hpp"

namespace rtype::client {

/** @brief Where the leaderboard lays its parts at one text size: its panel,
 * its tabs, its list, the row of its chips with the summary of the filter,
 * the bar that joins a server, and the lines it writes in place of rows. */
struct LeaderboardFrame {
  TextMetrics metrics;
  sf::IntRect panel;
  sf::Vector2i tabsTopLeft;
  int tabsWidth{};
  ItemListLayout list;
  int chipsTop{};
  int chipsRight{};
  sf::Vector2i summaryAnchor;
  sf::IntRect joinBar;
  sf::Vector2i messageLine;
  sf::Vector2i messageDetail;
  sf::Vector2i asking;
};

}  // namespace rtype::client
