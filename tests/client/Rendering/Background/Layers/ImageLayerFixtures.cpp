#include "ImageLayerFixtures.hpp"
#include <SFML/System/Vector2.hpp>
#include <string>
#include <vector>
#include "ImageContent.hpp"
#include "ImageLayer.hpp"
#include "ImageLayerTestConstants.hpp"
#include "LayerFrame.hpp"
#include "LayerTestConstants.hpp"
#include "RecordedFrame.hpp"
#include "RecordingLayerCanvas.hpp"

using rtype::client::ImageContent;
using rtype::client::ImageLayer;
using rtype::client::LayerFrame;

ImageLayer planetLayer(float repeat) {
  return ImageLayer{
      ImageContent{.file = std::string{PLANET_IMAGE}, .topLeft = PLANET_PLACE},
      repeat};
}

std::vector<RecordedFrame> framesPaintedBy(const ImageLayer& layer,
                                           const LayerFrame& frame) {
  RecordingLayerCanvas canvas;
  layer.paint(canvas, frame);
  return canvas.frames();
}

std::vector<sf::Vector2i> cornersOnTheBackground(
    const std::vector<RecordedFrame>& frames) {
  std::vector<sf::Vector2i> corners;
  for (const RecordedFrame& frame : frames) {
    if (frame.topLeft.x >= 0 &&
        static_cast<float>(frame.topLeft.x) < WIDTH_OF_THE_BACKGROUND) {
      corners.push_back(frame.topLeft);
    }
  }
  return corners;
}
