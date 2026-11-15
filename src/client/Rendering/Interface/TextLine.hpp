#pragma once

#include <SFML/System/Vector2.hpp>
#include <string_view>
#include "TextMetrics.hpp"
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
  /** @brief How large the player chose the text: a line is written at the
   * larger of this size and its style's scale. */
  TextMetrics metrics{};
  /** @brief Whether an outlined line is laid on an opaque plate instead of
   * its outline, as HIGH CONTRAST asks. */
  bool plated = false;
};

}  // namespace rtype::client
