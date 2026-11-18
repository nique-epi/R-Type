#include "StripFrameRect.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include "StripFrame.hpp"

namespace rtype::client {

sf::FloatRect stripFrameRect(sf::Vector2u imageSize, const StripFrame& frame) {
  if (frame.frames == 0) {
    return {};
  }
  const float width = std::floor(static_cast<float>(imageSize.x) /
                                 static_cast<float>(frame.frames));
  return sf::FloatRect{{static_cast<float>(frame.frame) * width, 0.0F},
                       {width, static_cast<float>(imageSize.y)}};
}

}  // namespace rtype::client
