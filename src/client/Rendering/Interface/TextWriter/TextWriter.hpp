#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <optional>
#include <string_view>
#include "LoadedFont.hpp"
#include "Logger.hpp"
#include "OutlinedText.hpp"
#include "TextLine.hpp"
#include "TextPlacement.hpp"
#include "TextStyle.hpp"

namespace rtype::client {

class AssetLibrary;

/**
 * @brief Writes lines of text with the two pixel fonts of the interface, on
 * whole pixels of a picture whose view is in its own pixels.
 *
 * A dense picture, whose texture gives each of its pixels CRISP_TEXT_DENSITY
 * pixels on each axis, gets text drawn at that many texture pixels per pixel
 * of the font, so a font pixel always covers whole texture pixels, even half
 * again as large. A line is written at the larger of the size the player
 * chose (TextLine::metrics) and the scale of its style.
 *
 * Text is written at TEXT_CHARACTER_SIZE, the size the fonts are designed for.
 * A line is placed as the browser places a line as tall as its font: its
 * baseline sits REGULAR_FONT_BASELINE_ROW or SMALL_FONT_BASELINE_ROW below the
 * anchor, while SFML puts the baseline of a text TEXT_CHARACTER_SIZE below its
 * position. An outlined line is first drawn in INK one pixel away in each of
 * the eight directions of CROSS_DIRECTIONS and DIAGONAL_DIRECTIONS, or laid
 * on a black plate when the line asks for one (TextLine::plated). Text is
 * drawn through CRISP_TEXT_FRAGMENT_SHADER, so its pixels are either lit or
 * transparent; where shaders are missing or the shader does not compile, it is
 * drawn smoothed, and the reason is logged once.
 *
 * The menus write through InterfaceCanvas, the figures of a gain through the
 * entity layer: each holds one. The fonts come from an asset library, which
 * must outlive the writer and hold them loaded when it is built.
 */
class TextWriter {
 public:
  /**
   * @throws AssetNotLoadedException when a font of the interface is not
   * loaded.
   */
  explicit TextWriter(const AssetLibrary& library);

  /** @brief Writes the line on @p target, on whole pixels. */
  void write(sf::RenderTarget& target, const TextLine& line) const;

  /** @returns How wide a line of UTF-8 text is in a font, in pixels, as
   * write() places it. */
  [[nodiscard]] int width(std::string_view text, TextFont font) const;

 private:
  /** @brief Draws @p text, already placed, in INK one pixel away in each of
   * the eight directions, at the opacity of its color. */
  static void drawOutline(sf::RenderTarget& target, sf::Text& text,
                          OutlinedText outlined);

  /** @brief Lays under @p text, already placed, a black plate PLATE_MARGIN
   * larger on each side, at @p opacity, as HIGH CONTRAST asks for a line
   * written over the game. */
  static void drawPlate(sf::RenderTarget& target, const sf::Text& text,
                        std::uint8_t opacity);

  [[nodiscard]] const LoadedFont& loadedFontOf(TextFont font) const;

  /** @returns A line of text in a font, at TEXT_CHARACTER_SIZE times
   * @p scale pixels of the target's texture, not placed. */
  [[nodiscard]] sf::Text lineOf(std::string_view text, TextFont font,
                                int scale) const;

  /** @returns Where SFML places @p text, in pixels of the view, so the line
   * lands as write() says: its baseline under the anchor, shifted left by its
   * width or half of it, counted in whole font pixels, when aligned right or
   * centered; on whole pixels of the texture. */
  [[nodiscard]] sf::Vector2f positionOf(const sf::Text& text,
                                        const TextLine& line,
                                        TextPlacement placement) const;

  LoadedFont regularFont_;
  LoadedFont smallFont_;
  logging::Logger logger_;
  std::optional<sf::Shader> crispText_;
};

}  // namespace rtype::client
