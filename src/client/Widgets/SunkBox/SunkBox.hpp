#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <cstdint>
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/**
 * @brief A box sunk into its panel: FIELD_INSIDE_COLOR inside, its top and
 * left sides in shade and its bottom and right sides lit, or all four lit in
 * SELECTED_FIELD_BORDER_COLOR, as on a selected field or an open list.
 */
class SunkBox {
 public:
  enum class Sides : std::uint8_t {
    Shaded,
    Lit,
  };

  SunkBox(sf::IntRect area, Sides sides);

  void paint(IInterfaceCanvas& canvas) const;

 private:
  sf::IntRect area_;
  Sides sides_;
};

}  // namespace rtype::client
