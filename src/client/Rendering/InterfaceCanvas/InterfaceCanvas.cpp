#include "InterfaceCanvas.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/String.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <cmath>
#include <optional>
#include <string>
#include <string_view>
#include "AssetLibrary.hpp"
#include "BackgroundConstants.hpp"
#include "CrispTextShader.hpp"
#include "FragmentShader.hpp"
#include "InterfaceColors.hpp"
#include "InterfaceConstants.hpp"
#include "RenderingConstants.hpp"
#include "TextStyle.hpp"

namespace rtype::client {

InterfaceCanvas::InterfaceCanvas(const AssetLibrary& library)
    : textures_(library),
      regularFont_{.font = &library.font(REGULAR_FONT_ID),
                   .baselineRow = REGULAR_FONT_BASELINE_ROW},
      smallFont_{.font = &library.font(SMALL_FONT_ID),
                 .baselineRow = SMALL_FONT_BASELINE_ROW},
      logger_(std::string(INTERFACE_LOG_MODULE_NAME)),
      crispText_(loadFragmentShader(CRISP_TEXT_SHADER, logger_)) {}

void InterfaceCanvas::clear() {
  canvas_.canvas().clear(sf::Color::Transparent);
}

void InterfaceCanvas::fill(sf::IntRect area, sf::Color color) {
  const sf::Vector2f topLeft(area.position);
  const sf::Vector2f bottomRight(area.position + area.size);
  const std::array corners{
      sf::Vertex{.position = topLeft, .color = color},
      sf::Vertex{.position = {bottomRight.x, topLeft.y}, .color = color},
      sf::Vertex{.position = {topLeft.x, bottomRight.y}, .color = color},
      sf::Vertex{.position = bottomRight, .color = color}};
  canvas_.canvas().draw(corners.data(), corners.size(),
                        sf::PrimitiveType::TriangleStrip);
}

void InterfaceCanvas::write(std::string_view text, sf::Vector2i anchor,
                            TextStyle style) {
  const LoadedFont& font = loadedFontOf(style.font);
  sf::Text line(*font.font, sf::String::fromUtf8(text.begin(), text.end()),
                TEXT_CHARACTER_SIZE);
  sf::Vector2i position(anchor.x, anchor.y + font.baselineRow -
                                      static_cast<int>(TEXT_CHARACTER_SIZE));
  if (style.alignment != TextAlignment::Left) {
    const float width = line.findCharacterPos(line.getString().getSize()).x;
    position.x -= style.alignment == TextAlignment::Right
                      ? static_cast<int>(std::round(width))
                      : static_cast<int>(std::floor(width * HALF));
  }
  const sf::RenderStates states(crispText_.has_value() ? &*crispText_
                                                       : nullptr);
  if (style.outlined) {
    line.setFillColor(INK);
    for (const auto& directions : {CROSS_DIRECTIONS, DIAGONAL_DIRECTIONS}) {
      for (const sf::Vector2i offset : directions) {
        line.setPosition(sf::Vector2f(position + offset));
        canvas_.canvas().draw(line, states);
      }
    }
  }
  line.setFillColor(style.color);
  line.setPosition(sf::Vector2f(position));
  canvas_.canvas().draw(line, states);
}

void InterfaceCanvas::drawImage(std::string_view assetId, sf::IntRect source,
                                sf::Vector2i topLeft, int scale,
                                sf::IntRect visibleArea) {
  const sf::Texture* texture = textures_.texture(assetId);
  if (texture == nullptr) {
    if (!reportedMissingIds_.contains(assetId)) {
      reportedMissingIds_.emplace(assetId);
      logger_.warn("no texture for asset id ", assetId);
    }
    return;
  }
  const std::optional<sf::IntRect> shown =
      sf::IntRect(topLeft, source.size * scale).findIntersection(visibleArea);
  if (!shown.has_value()) {
    return;
  }
  const sf::Vector2i firstPixel =
      (shown->position - topLeft + sf::Vector2i(scale - 1, scale - 1)) / scale;
  const sf::Vector2i endPixel =
      (shown->position + shown->size - topLeft) / scale;
  const sf::Vector2i shownPixels = endPixel - firstPixel;
  if (shownPixels.x <= 0 || shownPixels.y <= 0) {
    return;
  }
  sf::Sprite sprite(*texture,
                    sf::IntRect(source.position + firstPixel, shownPixels));
  sprite.setPosition(sf::Vector2f(topLeft + (firstPixel * scale)));
  sprite.setScale({static_cast<float>(scale), static_cast<float>(scale)});
  canvas_.canvas().draw(sprite);
}

void InterfaceCanvas::display() { canvas_.canvas().display(); }

void InterfaceCanvas::show(sf::RenderTarget& target) const {
  canvas_.show(target);
}

const InterfaceCanvas::LoadedFont& InterfaceCanvas::loadedFontOf(
    TextFont font) const {
  switch (font) {
    case TextFont::Small:
      return smallFont_;
    case TextFont::Regular:
      break;
  }
  return regularFont_;
}

}  // namespace rtype::client
