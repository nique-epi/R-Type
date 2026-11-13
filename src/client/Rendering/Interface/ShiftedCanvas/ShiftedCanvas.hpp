#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <span>
#include <string_view>
#include "ColoredPixel.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "TextMetrics.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"

namespace sf {
class RenderTarget;
}  // namespace sf

namespace rtype::client {

/**
 * @brief Paints on another canvas, every drawing moved by an offset: a widget
 * laid out for its place in a list is painted where the list scrolled it.
 *
 * It only moves what is drawn; clear(), display() and show() reach the canvas
 * it paints on, which must outlive it.
 */
class ShiftedCanvas : public IInterfaceCanvas {
 public:
  /** @param offset How far every drawing moves, in pixels of the picture. */
  ShiftedCanvas(IInterfaceCanvas& canvas, sf::Vector2i offset);

  void clear() override;
  void fill(sf::IntRect area, sf::Color color) override;
  void plot(std::span<const ColoredPixel> pixels) override;
  void write(std::string_view text, sf::Vector2i anchor,
             TextStyle style) override;
  void drawImage(TextureArea image, ImagePlacement placement,
                 sf::IntRect visibleArea) override;
  void drawSmoothImage(std::string_view assetId, sf::IntRect area,
                       sf::Color tint) override;
  [[nodiscard]] int textWidth(std::string_view text,
                              TextFont font) const override;
  [[nodiscard]] TextMetrics textMetrics() const override;
  void display() override;
  void show(sf::RenderTarget& target) const override;

 private:
  IInterfaceCanvas* canvas_;
  sf::Vector2i offset_;
};

}  // namespace rtype::client
