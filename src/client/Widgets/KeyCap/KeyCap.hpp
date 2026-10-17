#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <string>
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/**
 * @brief A key seen from the front, its name on it: a KEY_CAP_FACE_COLOR face,
 * a one-pixel edge, a KEY_CAP_SIDE_THICKNESS lit side at the bottom, the name
 * KEY_CAP_PADDING in, CONTROL_HEIGHT tall.
 */
class KeyCap {
 public:
  /** @brief The edge: plain, lit on the selected row, gold on a conflict. */
  enum class Edge : std::uint8_t {
    Plain,
    Selected,
    Conflict,
  };

  KeyCap(std::string name, Edge edge);

  /** @returns Its width: its name and KEY_CAP_PADDING on both sides, at
   * least KEY_CAP_MINIMUM_WIDTH. */
  [[nodiscard]] int width() const;

  void paint(IInterfaceCanvas& canvas, sf::Vector2i topLeft) const;

 private:
  std::string name_;
  Edge edge_;
};

}  // namespace rtype::client
