#pragma once

#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/** @brief Where a plane of the sky lies: the whole background pixels the
 * shake of the camera moves it by, and how far right of the pixels it is
 * painted on it lies, between -1 and 1 excluded. */
struct PlanePlacement {
  sf::Vector2i cameraOffset;
  float shift{};
};

}  // namespace rtype::client
