#pragma once

#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/** @returns How wide @p count bars of @p barSize are side by side, @p gap
 * apart, in pixels. constexpr, so constants such as the size of the bays of
 * the waiting room are computed with it. */
constexpr int barRowWidth(int count, sf::Vector2i barSize, int gap) {
  return (count * barSize.x) + ((count - 1) * gap);
}

}  // namespace rtype::client
