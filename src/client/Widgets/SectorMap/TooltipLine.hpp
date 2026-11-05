#pragma once

#include <SFML/Graphics/Color.hpp>
#include <string>

namespace rtype::client {

/** @brief A line of small print in the bubble of a server, and its color. */
struct TooltipLine {
  std::string text;
  sf::Color color;
};

}  // namespace rtype::client
