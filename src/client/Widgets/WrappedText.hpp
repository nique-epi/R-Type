#pragma once

#include <SFML/System/Vector2.hpp>
#include <optional>
#include "TextStyle.hpp"

namespace rtype::client {

/** @brief Where and how a text is written by writeWrappedLines(): the anchor
 * of its first line, how wide it may run, none for one line, and its style,
 * whose font measures the lines. */
struct WrappedText {
  sf::Vector2i anchor;
  std::optional<int> width;
  TextStyle style;
};

}  // namespace rtype::client
