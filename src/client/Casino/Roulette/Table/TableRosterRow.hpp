#pragma once

#include <SFML/Graphics/Color.hpp>
#include <string>
#include "PlayerColors.hpp"

namespace rtype::client {

/** @brief A player seated at the table as its panel lists it: its color, its
 * name, and its stake or its gain, in the color it is written in. */
struct TableRosterRow {
  PlayerColors colors;
  std::string name;
  std::string value;
  sf::Color valueColor;
};

}  // namespace rtype::client
