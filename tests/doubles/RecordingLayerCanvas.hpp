#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <string_view>
#include <vector>
#include "ColoredPixel.hpp"
#include "ILayerCanvas.hpp"
#include "RecordedFrame.hpp"
#include "RecordedTiles.hpp"
#include "StripFrame.hpp"

/** @brief Layer canvas that remembers what it is asked to paint, in order;
 * it needs no graphics card. */
class RecordingLayerCanvas final : public rtype::client::ILayerCanvas {
 public:
  void plot(sf::Vector2i pixel, sf::Color color) override;
  void drawFrame(const rtype::client::StripFrame& frame,
                 sf::Vector2i topLeft) override;
  void drawTiles(std::string_view tileset, const sf::VertexArray& tiles,
                 int column) override;

  [[nodiscard]] const std::vector<rtype::client::ColoredPixel>& plots() const;
  [[nodiscard]] const std::vector<RecordedFrame>& frames() const;
  [[nodiscard]] const std::vector<RecordedTiles>& tiles() const;

 private:
  std::vector<rtype::client::ColoredPixel> plots_;
  std::vector<RecordedFrame> frames_;
  std::vector<RecordedTiles> tiles_;
};
