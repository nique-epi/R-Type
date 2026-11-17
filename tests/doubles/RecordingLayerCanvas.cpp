#include "RecordingLayerCanvas.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
#include <string_view>
#include <vector>
#include "ColoredPixel.hpp"
#include "RecordedFrame.hpp"
#include "RecordedTiles.hpp"
#include "StripFrame.hpp"

using rtype::client::ColoredPixel;
using rtype::client::StripFrame;

void RecordingLayerCanvas::plot(sf::Vector2i pixel, sf::Color color) {
  plots_.push_back(ColoredPixel{.position = pixel, .color = color});
}

void RecordingLayerCanvas::drawFrame(const StripFrame& frame,
                                     sf::Vector2i topLeft) {
  frames_.push_back(RecordedFrame{.file = std::string{frame.file},
                                  .frames = frame.frames,
                                  .frame = frame.frame,
                                  .topLeft = topLeft});
}

void RecordingLayerCanvas::drawTiles(std::string_view tileset,
                                     const sf::VertexArray& tiles, int column) {
  tiles_.push_back(RecordedTiles{.tileset = std::string{tileset},
                                 .vertices = tiles.getVertexCount(),
                                 .column = column});
}

const std::vector<ColoredPixel>& RecordingLayerCanvas::plots() const {
  return plots_;
}

const std::vector<RecordedFrame>& RecordingLayerCanvas::frames() const {
  return frames_;
}

const std::vector<RecordedTiles>& RecordingLayerCanvas::tiles() const {
  return tiles_;
}
