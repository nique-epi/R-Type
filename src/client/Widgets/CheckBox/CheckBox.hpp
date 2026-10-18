#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include "CheckMark.hpp"
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/**
 * @brief The CHECK_BOX_SIZE square of a choice among others: its outline,
 * solid, filled but for CHECK_MARK_MARGIN when checked, or dashed when the
 * choice cannot be taken.
 */
class CheckBox {
 public:
  CheckBox(sf::Vector2i topLeft, CheckMark mark);

  void paint(IInterfaceCanvas& canvas, sf::Color color) const;

 private:
  sf::Vector2i topLeft_;
  CheckMark mark_;
};

}  // namespace rtype::client
