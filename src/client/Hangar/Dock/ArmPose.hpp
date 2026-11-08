#pragma once

#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/** @brief What an arm does in a frame: where its jaws close, how far they
 * are open, 0 to 1, whether they grip the hull (its lights turn on), and
 * whether they carry an item. */
struct ArmPose {
  sf::Vector2i target;
  float open{};
  bool gripping{};
  bool carrying{};
};

}  // namespace rtype::client
