#include "RecordingPixelSurface.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>
#include "ColoredPixel.hpp"

void RecordingPixelSurface::plot(sf::Vector2i pixel, sf::Color color) {
  plots_.push_back({.position = pixel, .color = color});
}

const std::vector<rtype::client::ColoredPixel>& RecordingPixelSurface::plots()
    const {
  return plots_;
}
