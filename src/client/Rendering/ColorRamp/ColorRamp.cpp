#include "ColorRamp.hpp"
#include <SFML/Graphics/Color.hpp>
#include <algorithm>
#include <cstddef>
#include <span>
#include "FlashPreference.hpp"

namespace rtype::client {

sf::Color colorAt(std::span<const sf::Color> ramp, float share) {
  const auto index =
      static_cast<std::size_t>(share * static_cast<float>(ramp.size()));
  return ramp[std::min(index, ramp.size() - 1)];
}

std::span<const sf::Color> rampFor(std::span<const sf::Color> ramp,
                                   FlashPreference preference) {
  return preference == FlashPreference::Full || ramp.empty() ? ramp
                                                             : ramp.subspan(1);
}

}  // namespace rtype::client
