#include "InterfaceCanvas.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Glsl.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "AssetLibrary.hpp"
#include "BackgroundConstants.hpp"
#include "FragmentShader.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceConstants.hpp"
#include "PlayerColors.hpp"
#include "ShadeSwap.hpp"
#include "ShadeSwapShaderConstants.hpp"
#include "SmoothImage.hpp"
#include "TextLine.hpp"
#include "TextStyle.hpp"
#include "TextWriter.hpp"
#include "TextureArea.hpp"

namespace rtype::client {

namespace {

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

/** @returns The three shades of a player color as the shade swap shader reads
 * them, dark, main then light, each channel from 0 to 1. */
std::array<sf::Glsl::Vec4, PLAYER_SHADE_COUNT> shaderShadesOf(
    const PlayerColors& shades) {
  return {sf::Glsl::Vec4(shades.dark), sf::Glsl::Vec4(shades.ship),
          sf::Glsl::Vec4(shades.light)};
}

/** @brief Gives the shade swap shader the shades it looks for and those it
 * paints instead. */
void setShades(sf::Shader& shader, const ShadeSwap& swap) {
  const std::array sought = shaderShadesOf(swap.from);
  const std::array painted = shaderShadesOf(swap.to);
  shader.setUniformArray(std::string(SHADE_SWAP_FROM_UNIFORM), sought.data(),
                         sought.size());
  shader.setUniformArray(std::string(SHADE_SWAP_TO_UNIFORM), painted.data(),
                         painted.size());
}

/** @returns How a copy is drawn: through the shade swap shader, given the
 * swap, when the copy asks for one and the shader is loaded; plainly
 * otherwise. */
sf::RenderStates statesFor(const std::optional<ShadeSwap>& recolor,
                           std::optional<sf::Shader>& shader) {
  if (!recolor.has_value() || !shader.has_value()) {
    return sf::RenderStates::Default;
  }
  setShades(*shader, *recolor);
  return {&*shader};
}

}  // namespace

InterfaceCanvas::InterfaceCanvas(const AssetLibrary& library)
    : textures_(library),
      writer_(library),
      logger_(std::string(INTERFACE_LOG_MODULE_NAME)),
      shadeSwap_(loadFragmentShader(SHADE_SWAP_SHADER, logger_)) {}

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
  writer_.write(canvas_.canvas(),
                TextLine{.text = text, .anchor = anchor, .style = style});
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
  sprite.setColor(placement.tint);
  canvas_.canvas().draw(sprite, statesFor(placement.recolor, shadeSwap_));
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
  return writer_.width(text, font);
}

void InterfaceCanvas::display() {
  canvas_.canvas().display();
  shownSmoothImages_ = paintedSmoothImages_;
}

void InterfaceCanvas::show(sf::RenderTarget& target) const {
  canvas_.show(target);
  showSmoothImages(target);
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
