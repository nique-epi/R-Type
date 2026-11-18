#pragma once

#include <SFML/System/Vector2.hpp>
#include <optional>

namespace rtype::client {

/** @brief What the wheel shows over its bowl: a frame of the turns of the
 * rotor, and the ball, its top-left corner from the top-left corner of the
 * wheel, when it is in play. */
struct WheelPicture {
  int rotorFrame{};
  std::optional<sf::Vector2i> ball = std::nullopt;

  bool operator==(const WheelPicture&) const = default;
};

}  // namespace rtype::client
