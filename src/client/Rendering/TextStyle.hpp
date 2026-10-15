#pragma once

#include <SFML/Graphics/Color.hpp>
#include <cstdint>

namespace rtype::client {

/** @brief The two pixel fonts of the interface, see REGULAR_FONT_ID and
 * SMALL_FONT_ID. */
enum class TextFont : std::uint8_t {
  Regular,
  Small,
};

/** @brief Which top corner of its line a text is anchored by. */
enum class TextAlignment : std::uint8_t {
  Left,
  Right,
};

/**
 * @brief How a line of text is written: its font, its color, whether it is
 * outlined in INK so it stays readable over the sky, and which side its anchor
 * is on.
 */
struct TextStyle {
  TextFont font{};
  sf::Color color;
  bool outlined = false;
  TextAlignment alignment = TextAlignment::Left;
};

}  // namespace rtype::client
