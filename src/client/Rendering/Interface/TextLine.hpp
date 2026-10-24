#pragma once

#include <SFML/System/Vector2.hpp>
#include <string_view>
#include "TextStyle.hpp"

namespace rtype::client {

/** @brief A line of text, where it goes and how it looks. */
struct TextLine {
  /** @brief UTF-8 text. */
  std::string_view text;
  /** @brief Top-left corner of the line, its top-right corner with
   * TextAlignment::Right, or the middle of its top edge with
   * TextAlignment::Center, in pixels of the picture. */
  sf::Vector2i anchor;
  TextStyle style;
};

}  // namespace rtype::client
