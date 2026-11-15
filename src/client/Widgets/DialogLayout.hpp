#pragma once

#include <SFML/System/Vector2.hpp>
#include <string>
#include <vector>

namespace rtype::client {

/** @brief Where a dialog lays its parts, from the top-left corner of its
 * panel: its size, its heading, the lines of its message and how far apart
 * they are, its rows, and its buttons. */
struct DialogLayout {
  sf::Vector2i size;
  std::vector<std::string> messageLines;
  int messageTop{};
  int messagePitch{};
  int rowsTop{};
  sf::Vector2i rowSize;
  int rowPitch{};
  int buttonsTop{};
  sf::Vector2i buttonSize;
};

}  // namespace rtype::client
