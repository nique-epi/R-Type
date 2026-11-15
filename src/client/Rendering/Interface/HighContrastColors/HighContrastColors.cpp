#include "HighContrastColors.hpp"
#include <SFML/Graphics/Color.hpp>
#include "ColorReplacement.hpp"
#include "HighContrastColorConstants.hpp"

namespace rtype::client {

sf::Color highContrastOf(sf::Color color) {
  for (const ColorReplacement& replacement : HIGH_CONTRAST_COLORS) {
    if (replacement.from.r == color.r && replacement.from.g == color.g &&
        replacement.from.b == color.b) {
      return {replacement.to.r, replacement.to.g, replacement.to.b, color.a};
    }
  }
  return color;
}

}  // namespace rtype::client
