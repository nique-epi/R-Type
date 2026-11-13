#include "TextWriter.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/String.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <optional>
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

/** @returns The view of @p target, cut to the pixels of @p clip, which lie
 * inside the view; the clip is in pixels of the view, whatever the density of
 * the target. */
sf::View clippedViewOf(const sf::RenderTarget& target, sf::IntRect clip) {
  sf::View view = target.getView();
  const sf::Vector2f size = view.getSize();
  const sf::Vector2f topLeft = view.getCenter() - (size * HALF);
  const sf::Vector2f position = sf::Vector2f(clip.position) - topLeft;
  const sf::Vector2f extent(clip.size);
  view.setScissor(sf::FloatRect({position.x / size.x, position.y / size.y},
                                {extent.x / size.x, extent.y / size.y}));
  return view;
}

/** @returns How many pixels of its texture @p target gives one pixel of its
 * view: CRISP_TEXT_DENSITY on a dense PixelCanvas, NATIVE_DENSITY otherwise. */
int densityOf(const sf::RenderTarget& target) {
  return static_cast<int>(std::lround(static_cast<float>(target.getSize().x) /
                                      target.getView().getSize().x));
}

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
      logger_(std::string(INTERFACE_LOGGER_NAME)),
      crispText_(loadFragmentShader(CRISP_TEXT_SHADER, logger_)) {}

void TextWriter::write(sf::RenderTarget& target, const TextLine& line) const {
  std::optional<sf::View> whole;
  if (line.style.clip.has_value()) {
    whole = target.getView();
    target.setView(clippedViewOf(target, *line.style.clip));
  }
  const int density = densityOf(target);
  sf::Text text =
      lineOf(line.text, line.style.font, line.style.scale * density);
  const sf::Vector2f position = positionOf(text, line, density);
  const float shrink = 1.0F / static_cast<float>(density);
  text.setScale({shrink, shrink});
  const sf::RenderStates states(crispText_.has_value() ? &*crispText_
                                                       : nullptr);
  if (line.style.outlined) {
    sf::Color outline = INK;
    outline.a = line.style.color.a;
    text.setFillColor(outline);
    for (const auto& directions : {CROSS_DIRECTIONS, DIAGONAL_DIRECTIONS}) {
      for (const sf::Vector2i offset : directions) {
        text.setPosition(position + sf::Vector2f(offset));
        target.draw(text, states);
      }
    }
  }
  text.setFillColor(line.style.color);
  text.setPosition(position);
  target.draw(text, states);
  if (whole.has_value()) {
    target.setView(*whole);
  }
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

sf::Vector2f TextWriter::positionOf(const sf::Text& text, const TextLine& line,
                                    int density) const {
  const int risingRows = loadedFontOf(line.style.font).baselineRow -
                         static_cast<int>(TEXT_CHARACTER_SIZE);
  sf::Vector2i position(
      line.anchor.x * density,
      (line.anchor.y + (risingRows * line.style.scale)) * density);
  if (line.style.alignment != TextAlignment::Left) {
    const float lineWidth = widthOf(text);
    position.x -= line.style.alignment == TextAlignment::Right
                      ? static_cast<int>(std::round(lineWidth))
                      : static_cast<int>(std::floor(
                            lineWidth / static_cast<float>(density) * HALF)) *
                            density;
  }
  return sf::Vector2f(position) / static_cast<float>(density);
}

}  // namespace rtype::client
