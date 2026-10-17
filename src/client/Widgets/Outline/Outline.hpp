#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <cstdint>
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/** @brief The one-pixel outline of a rectangle, solid, or dashed: one pixel
 * in DASH_PERIOD lit, from the top-left corner. */
class Outline {
 public:
  enum class Line : std::uint8_t {
    Solid,
    Dashed,
  };

  Outline(sf::IntRect area, Line line);

  void paint(IInterfaceCanvas& canvas, sf::Color color) const;

 private:
  sf::IntRect area_;
  Line line_;
};

}  // namespace rtype::client
