#pragma once

#include <SFML/Graphics/Color.hpp>
#include <string>
#include "PlayerColors.hpp"

namespace rtype::client {

/** @brief A pilot seated at the table as the panel TABLE lists it: its color,
 * its name, whether it asked to spin, and its stake or its gain, in the
 * color it is written in. */
struct TableRosterRow {
  PlayerColors colors;
  std::string name;
  bool spinAsked{};
  std::string value;
  sf::Color valueColor;
};

}  // namespace rtype::client
