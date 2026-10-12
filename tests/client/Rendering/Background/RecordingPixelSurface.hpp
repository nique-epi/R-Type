#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>
#include "ColoredPixel.hpp"
#include "IPixelSurface.hpp"

/**
 * @brief Pixel surface that remembers every plot, in order.
 */
class RecordingPixelSurface final : public rtype::client::IPixelSurface {
 public:
  void plot(sf::Vector2i pixel, sf::Color color) override;

  [[nodiscard]] const std::vector<rtype::client::ColoredPixel>& plots() const;

 private:
  std::vector<rtype::client::ColoredPixel> plots_;
};
