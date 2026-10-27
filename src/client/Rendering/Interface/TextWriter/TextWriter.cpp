#include "TextWriter.hpp"
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/String.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <string>
#include <string_view>
#include "AssetLibrary.hpp"
#include "BackgroundConstants.hpp"
#include "CrispTextShaderConstants.hpp"
#include "FragmentShader.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "LoadedFont.hpp"
#include "RenderingConstants.hpp"
#include "TextLine.hpp"
#include "TextStyle.hpp"

namespace rtype::client {

namespace {

/** @returns How wide a line is, in pixels: where a character after its last
 * would start. */
float widthOf(const sf::Text& line) {
  return line.findCharacterPos(line.getString().getSize()).x;
}

}  // namespace

TextWriter::TextWriter(const AssetLibrary& library)
    : regularFont_{.font = &library.font(REGULAR_FONT_ID),
                   .baselineRow = REGULAR_FONT_BASELINE_ROW},
      smallFont_{.font = &library.font(SMALL_FONT_ID),
                 .baselineRow = SMALL_FONT_BASELINE_ROW},
      logger_(std::string(INTERFACE_LOG_MODULE_NAME)),
      crispText_(loadFragmentShader(CRISP_TEXT_SHADER, logger_)) {}

void TextWriter::write(sf::RenderTarget& target, const TextLine& line) const {
  sf::Text text = lineOf(line.text, line.style.font, line.style.scale);
  const sf::Vector2i position = positionOf(text, line);
  const sf::RenderStates states(crispText_.has_value() ? &*crispText_
                                                       : nullptr);
  if (line.style.outlined) {
    text.setFillColor(INK);
    for (const auto& directions : {CROSS_DIRECTIONS, DIAGONAL_DIRECTIONS}) {
      for (const sf::Vector2i offset : directions) {
        text.setPosition(sf::Vector2f(position + offset));
        target.draw(text, states);
      }
    }
  }
  text.setFillColor(line.style.color);
  text.setPosition(sf::Vector2f(position));
  target.draw(text, states);
}

int TextWriter::width(std::string_view text, TextFont font) const {
  return static_cast<int>(
      std::round(widthOf(lineOf(text, font, NATIVE_SCALE))));
}

const LoadedFont& TextWriter::loadedFontOf(TextFont font) const {
  switch (font) {
    case TextFont::Small:
      return smallFont_;
    case TextFont::Regular:
      break;
  }
  return regularFont_;
}

sf::Text TextWriter::lineOf(std::string_view text, TextFont font,
                            int scale) const {
  return {*loadedFontOf(font).font,
          sf::String::fromUtf8(text.begin(), text.end()),
          TEXT_CHARACTER_SIZE * static_cast<unsigned int>(scale)};
}

sf::Vector2i TextWriter::positionOf(const sf::Text& text,
                                    const TextLine& line) const {
  const int risingRows = loadedFontOf(line.style.font).baselineRow -
                         static_cast<int>(TEXT_CHARACTER_SIZE);
  sf::Vector2i position(line.anchor.x,
                        line.anchor.y + (risingRows * line.style.scale));
  if (line.style.alignment != TextAlignment::Left) {
    const float lineWidth = widthOf(text);
    position.x -= line.style.alignment == TextAlignment::Right
                      ? static_cast<int>(std::round(lineWidth))
                      : static_cast<int>(std::floor(lineWidth * HALF));
  }
  return position;
}

}  // namespace rtype::client
