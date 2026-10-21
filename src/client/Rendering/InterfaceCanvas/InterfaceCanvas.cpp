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
#include <vector>
#include "AssetLibrary.hpp"
#include "BackgroundConstants.hpp"
#include "CrispTextShader.hpp"
#include "FragmentShader.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceColors.hpp"
#include "InterfaceConstants.hpp"
#include "RenderingConstants.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"

namespace rtype::client {

namespace {

/** @returns How wide a line is, in pixels: where a character after its last
 * would start. */
float widthOf(const sf::Text& line) {
  return line.findCharacterPos(line.getString().getSize()).x;
}

/** @returns The pixels of an image that its copy, enlarged and placed as
 * asked, shows whole inside the visible area, from the top-left corner of
 * the image; nothing when it shows none. */
std::optional<sf::IntRect> visiblePixelsOf(TextureArea image,
                                           ImagePlacement placement,
                                           sf::IntRect visibleArea) {
  const sf::Vector2i topLeft = placement.topLeft;
  const int scale = placement.scale;
  const std::optional<sf::IntRect> shown =
      sf::IntRect(topLeft, image.rectangle.size * scale)
          .findIntersection(visibleArea);
  if (!shown.has_value()) {
    return std::nullopt;
  }
  const sf::Vector2i firstPixel =
      (shown->position - topLeft + sf::Vector2i(scale - 1, scale - 1)) / scale;
  const sf::Vector2i endPixel =
      (shown->position + shown->size - topLeft) / scale;
  const sf::Vector2i shownPixels = endPixel - firstPixel;
  if (shownPixels.x <= 0 || shownPixels.y <= 0) {
    return std::nullopt;
  }
  return sf::IntRect(firstPixel, shownPixels);
}

}  // namespace

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
  paintedSmoothImages_.clear();
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
  sf::Text line = lineOf(text, style.font);
  sf::Vector2i position(anchor.x, anchor.y + font.baselineRow -
                                      static_cast<int>(TEXT_CHARACTER_SIZE));
  if (style.alignment != TextAlignment::Left) {
    const float width = widthOf(line);
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

void InterfaceCanvas::drawImage(TextureArea image, ImagePlacement placement,
                                sf::IntRect visibleArea) {
  const sf::Texture* texture = foundTexture(image.assetId);
  const std::optional<sf::IntRect> visible =
      visiblePixelsOf(image, placement, visibleArea);
  if (texture == nullptr || !visible.has_value()) {
    return;
  }
  const auto scale = static_cast<float>(placement.scale);
  sf::Sprite sprite(
      *texture,
      sf::IntRect(image.rectangle.position + visible->position, visible->size));
  sprite.setPosition(
      sf::Vector2f(placement.topLeft + (visible->position * placement.scale)));
  sprite.setScale({scale, scale});
  canvas_.canvas().draw(sprite);
}

void InterfaceCanvas::drawSmoothImage(std::string_view assetId,
                                      sf::IntRect area, sf::Color tint) {
  const sf::Texture* texture = foundTexture(assetId);
  if (texture == nullptr) {
    return;
  }
  paintedSmoothImages_.push_back({.texture = &smoothCopyOf(assetId, *texture),
                                  .area = area,
                                  .tint = tint});
}

int InterfaceCanvas::textWidth(std::string_view text, TextFont font) const {
  return static_cast<int>(std::round(widthOf(lineOf(text, font))));
}

void InterfaceCanvas::display() {
  canvas_.canvas().display();
  shownSmoothImages_ = paintedSmoothImages_;
}

void InterfaceCanvas::show(sf::RenderTarget& target) const {
  canvas_.show(target);
  showSmoothImages(target);
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

sf::Text InterfaceCanvas::lineOf(std::string_view text, TextFont font) const {
  return {*loadedFontOf(font).font,
          sf::String::fromUtf8(text.begin(), text.end()), TEXT_CHARACTER_SIZE};
}

const sf::Texture* InterfaceCanvas::foundTexture(std::string_view assetId) {
  const sf::Texture* texture = textures_.texture(assetId);
  if (texture == nullptr && !reportedMissingIds_.contains(assetId)) {
    reportedMissingIds_.emplace(assetId);
    logger_.warn("no texture for asset id ", assetId);
  }
  return texture;
}

const sf::Texture& InterfaceCanvas::smoothCopyOf(std::string_view assetId,
                                                 const sf::Texture& texture) {
  const auto known = smoothTextures_.find(assetId);
  return known != smoothTextures_.end() ? known->second
                                        : addSmoothCopy(assetId, texture);
}

const sf::Texture& InterfaceCanvas::addSmoothCopy(std::string_view assetId,
                                                  const sf::Texture& texture) {
  sf::Texture& copy =
      smoothTextures_.emplace(std::string(assetId), texture).first->second;
  copy.setSmooth(true);
  if (!copy.generateMipmap()) {
    logger_.warn("no mipmaps for asset id ", assetId, ", smoothed without");
  }
  return copy;
}

void InterfaceCanvas::showSmoothImages(sf::RenderTarget& target) const {
  for (const SmoothImage& image : shownSmoothImages_) {
    const sf::Vector2f textureSize(image.texture->getSize());
    const sf::Vector2f areaSize =
        sf::Vector2f(image.area.size) * BACKGROUND_PIXEL_SIZE;
    sf::Sprite sprite(*image.texture);
    sprite.setPosition(sf::Vector2f(image.area.position) *
                       BACKGROUND_PIXEL_SIZE);
    sprite.setScale({areaSize.x / textureSize.x, areaSize.y / textureSize.y});
    sprite.setColor(image.tint);
    target.draw(sprite);
  }
}

}  // namespace rtype::client
