#include "TailGradient.hpp"
#include <SFML/Graphics/Color.hpp>

namespace rtype::client {

sf::Color tailColor(const TailGradient& gradient, float fraction) {
  for (const GradientStop& stop : gradient) {
    if (fraction < stop.end) {
      return stop.color;
    }
  }
  return gradient.back().color;
}

}  // namespace rtype::client
