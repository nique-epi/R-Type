#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <string_view>

namespace rtype::client {

/** @brief The text inside the box of a field: the box, the text, its color,
 * and whether the caret follows it. */
struct FieldText {
  sf::IntRect box;
  std::string_view text;
  sf::Color color;
  bool caret = false;
};

}  // namespace rtype::client
