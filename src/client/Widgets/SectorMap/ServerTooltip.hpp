#pragma once

#include <SFML/System/Vector2.hpp>
#include <string>
#include <vector>
#include "TextMetrics.hpp"
#include "TooltipLine.hpp"

namespace rtype::client {

/** @brief The bubble next to the chosen server: where it starts, how wide it
 * is, its title in cyan, then its lines of small print, and the text size it
 * is written at, which spaces its lines. */
struct ServerTooltip {
  sf::Vector2i topLeft;
  int width{};
  std::string title;
  std::vector<TooltipLine> lines;
  TextMetrics metrics{};
};

}  // namespace rtype::client
