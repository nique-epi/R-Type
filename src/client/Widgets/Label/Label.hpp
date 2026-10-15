#pragma once

#include <SFML/System/Vector2.hpp>
#include <string>
#include "IInterfaceCanvas.hpp"
#include "TextStyle.hpp"

namespace rtype::client {

/** @brief A line of text of a menu, already placed. */
class Label {
 public:
  /**
   * @param text UTF-8 text.
   * @param anchor Top-left corner of the line, or its top-right corner with
   * TextAlignment::Right, in pixels of the interface picture.
   */
  Label(std::string text, sf::Vector2i anchor, TextStyle style);

  void paint(IInterfaceCanvas& canvas) const;

 private:
  std::string text_;
  sf::Vector2i anchor_;
  TextStyle style_;
};

}  // namespace rtype::client
