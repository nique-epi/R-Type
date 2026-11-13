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
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include "AssetLibrary.hpp"
#include "BackgroundConstants.hpp"
#include "ColoredPixel.hpp"
#include "FragmentShader.hpp"
#include "HighContrastColors.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceConstants.hpp"
#include "InterfaceLook.hpp"
#include "InterfaceLookSource.hpp"
#include "PlayerColors.hpp"
#include "ShadeSwap.hpp"
#include "ShadeSwapShaderConstants.hpp"
#include "SmoothImageSpot.hpp"
#include "TextLine.hpp"
#include "TextMeasures.hpp"
#include "TextMetrics.hpp"
#include "TextStyle.hpp"
#include "TextWriter.hpp"
#include "TextureArea.hpp"

namespace rtype::client {

namespace {

/** @returns The pixels of an image that its copy, enlarged and placed as
 * asked, shows whole inside the visible area, from the top-left corner of
 * the image; nothing when it shows none. */
/** @returns The look @p source gives now, or the look of a first launch when
 * the canvas was given no source. */
InterfaceLook lookOf(const InterfaceLookSource& source) {
  return source ? source() : InterfaceLook{};
}

/** @returns The color @p color is drawn in under @p look: its high contrast
 * replacement when HIGH CONTRAST is on, itself otherwise. */
sf::Color shownColorOf(const InterfaceLook& look, sf::Color color) {
  return look.highContrast ? highContrastOf(color) : color;
}

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

InterfaceCanvas::InterfaceCanvas(const AssetLibrary& library,
                                 InterfaceLookSource look)
    : look_(std::move(look)),
      textures_(library),
      foundTextures_(textures_, INTERFACE_LOGGER_NAME),
      writer_(library),
      canvas_(CRISP_TEXT_DENSITY),
      logger_(std::string(INTERFACE_LOGGER_NAME)),
      shadeSwap_(loadFragmentShader(SHADE_SWAP_SHADER, logger_)) {}

void InterfaceCanvas::clear() {
  canvas_.canvas().clear(sf::Color::Transparent);
  smoothImages_.clear();
}

void InterfaceCanvas::fill(sf::IntRect area, sf::Color shade) {
  const sf::Color color = shownColorOf(lookOf(look_), shade);
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

void InterfaceCanvas::plot(std::span<const ColoredPixel> pixels) {
  const InterfaceLook look = lookOf(look_);
  sf::VertexArray triangles{sf::PrimitiveType::Triangles};
  for (const ColoredPixel& pixel : pixels) {
    const sf::Color color = shownColorOf(look, pixel.color);
    const sf::Vector2f topLeft(pixel.position);
    const sf::Vector2f bottomRight = topLeft + sf::Vector2f{1.0F, 1.0F};
    const sf::Vector2f topRight{bottomRight.x, topLeft.y};
    const sf::Vector2f bottomLeft{topLeft.x, bottomRight.y};
    for (const sf::Vector2f corner :
         {topLeft, topRight, bottomLeft, topRight, bottomRight, bottomLeft}) {
      triangles.append(sf::Vertex{.position = corner, .color = color});
    }
  }
  canvas_.canvas().draw(triangles);
}

void InterfaceCanvas::write(std::string_view text, sf::Vector2i anchor,
                            TextStyle style) {
  const InterfaceLook look = lookOf(look_);
  style.color = shownColorOf(look, style.color);
  writer_.write(canvas_.canvas(), TextLine{.text = text,
                                           .anchor = anchor,
                                           .style = style,
                                           .metrics = look.text,
                                           .plated = look.highContrast});
}

void InterfaceCanvas::drawImage(TextureArea image, ImagePlacement placement,
                                sf::IntRect visibleArea) {
  const sf::Texture* texture = foundTextures_.found(image.assetId);
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
  const sf::Texture* texture = foundTextures_.found(assetId);
  if (texture == nullptr) {
    return;
  }
  smoothImages_.lay(assetId, *texture,
                    SmoothImageSpot{.area = area, .tint = tint});
}

int InterfaceCanvas::textWidth(std::string_view text, TextFont font) const {
  return widthAtSizeOf(textMetrics(), writer_.width(text, font));
}

TextMetrics InterfaceCanvas::textMetrics() const { return lookOf(look_).text; }

void InterfaceCanvas::display() {
  canvas_.canvas().display();
  smoothImages_.display();
}

void InterfaceCanvas::show(sf::RenderTarget& target) const {
  canvas_.show(target);
  smoothImages_.show(target);
}

}  // namespace rtype::client
