#pragma once

#include <SFML/Graphics/Color.hpp>
#include <cstdint>
#include "InterfaceConstants.hpp"

namespace rtype::client {

/** @brief The two pixel fonts of the interface, see REGULAR_FONT_ID and
 * SMALL_FONT_ID. */
enum class TextFont : std::uint8_t {
  Regular,
  Small,
};

/** @brief Which point of the top edge of its line a text is anchored by. */
enum class TextAlignment : std::uint8_t {
  Left,
  Right,
  /** @brief The middle of the top edge, rounded down to a whole pixel. */
  Center,
};

/**
 * @brief How a line of text is written: its font, its color, whether it is
 * outlined in INK so it stays readable over the sky, which side its anchor
 * is on, and how large it is.
 */
struct TextStyle {
  TextFont font{};
  sf::Color color;
  bool outlined = false;
  TextAlignment alignment = TextAlignment::Left;
  /** @brief Whole factor every pixel of the letters is enlarged by: the font
   * is drawn at that many times TEXT_CHARACTER_SIZE; the outline stays one
   * pixel wide. */
  int scale = NATIVE_SCALE;
};

}  // namespace rtype::client
