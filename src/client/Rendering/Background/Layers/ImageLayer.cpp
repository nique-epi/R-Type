#include "ImageLayer.hpp"
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <utility>
#include "BackgroundConstants.hpp"
#include "ILayerCanvas.hpp"
#include "ImageContent.hpp"
#include "LayerFrame.hpp"
#include "PixelRounding.hpp"
#include "StripFrame.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The column of the leftmost copy of an image that lies on
 * @p column and shows again every @p repeat columns, a width above zero: the
 * one copy that starts less than @p repeat columns left of the background. */
int leftmostCopy(int column, int repeat) {
  const auto laps =
      static_cast<int>(std::floor(static_cast<double>(column) / repeat));
  return column - ((laps + 1) * repeat);
}

}  // namespace

ImageLayer::ImageLayer(ImageContent image, float repeat)
    : image_(std::move(image)), repeat_(static_cast<int>(repeat)) {}

void ImageLayer::advance([[maybe_unused]] engine::Duration elapsed,
                         [[maybe_unused]] const LayerFrame& frame) {}

void ImageLayer::paint(ILayerCanvas& canvas, const LayerFrame& frame) const {
  const sf::Vector2i topLeft =
      nearestPixel(image_.topLeft) + sf::Vector2i(frame.column, 0);
  const StripFrame whole{.file = image_.file};
  if (repeat_ <= 0) {
    canvas.drawFrame(whole, topLeft);
    return;
  }
  for (int column = leftmostCopy(topLeft.x, repeat_);
       column < static_cast<int>(BACKGROUND_WIDTH); column += repeat_) {
    canvas.drawFrame(whole, {column, topLeft.y});
  }
}

int ImageLayer::leftEdge() const { return nearestPixel(image_.topLeft).x; }

}  // namespace rtype::client
