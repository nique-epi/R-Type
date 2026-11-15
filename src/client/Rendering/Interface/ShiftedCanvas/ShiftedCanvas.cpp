#include "ShiftedCanvas.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <span>
#include <string_view>
#include <vector>
#include "ColoredPixel.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "TextMetrics.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"

namespace rtype::client {

ShiftedCanvas::ShiftedCanvas(IInterfaceCanvas& canvas, sf::Vector2i offset)
    : canvas_(&canvas), offset_(offset) {}

void ShiftedCanvas::clear() { canvas_->clear(); }

void ShiftedCanvas::fill(sf::IntRect area, sf::Color color) {
  canvas_->fill(sf::IntRect(area.position + offset_, area.size), color);
}

void ShiftedCanvas::plot(std::span<const ColoredPixel> pixels) {
  std::vector<ColoredPixel> moved(pixels.begin(), pixels.end());
  for (ColoredPixel& pixel : moved) {
    pixel.position += offset_;
  }
  canvas_->plot(moved);
}

void ShiftedCanvas::write(std::string_view text, sf::Vector2i anchor,
                          TextStyle style) {
  if (style.clip.has_value()) {
    style.clip->position += offset_;
  }
  canvas_->write(text, anchor + offset_, style);
}

void ShiftedCanvas::drawImage(TextureArea image, ImagePlacement placement,
                              sf::IntRect visibleArea) {
  placement.topLeft += offset_;
  canvas_->drawImage(
      image, placement,
      sf::IntRect(visibleArea.position + offset_, visibleArea.size));
}

void ShiftedCanvas::drawSmoothImage(std::string_view assetId, sf::IntRect area,
                                    sf::Color tint) {
  canvas_->drawSmoothImage(
      assetId, sf::IntRect(area.position + offset_, area.size), tint);
}

int ShiftedCanvas::textWidth(std::string_view text, TextFont font) const {
  return canvas_->textWidth(text, font);
}

TextMetrics ShiftedCanvas::textMetrics() const {
  return canvas_->textMetrics();
}

void ShiftedCanvas::display() { canvas_->display(); }

void ShiftedCanvas::show(sf::RenderTarget& target) const {
  canvas_->show(target);
}

}  // namespace rtype::client
