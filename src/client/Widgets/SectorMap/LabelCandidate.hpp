#pragma once

#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/** @brief A star whose name may be written: the pixel it stands on, and how
 * wide its name is, in pixels. */
struct LabelCandidate {
  sf::Vector2i point;
  int nameWidth{};
};

}  // namespace rtype::client
