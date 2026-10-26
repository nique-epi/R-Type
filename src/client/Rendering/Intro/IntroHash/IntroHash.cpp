#include "IntroHash.hpp"
#include <cmath>
#include "IntroPaintConstants.hpp"

namespace rtype::client {

float introHash(double seed) {
  const double wave =
      std::sin((seed * INTRO_HASH_FREQUENCY) + INTRO_HASH_PHASE) *
      INTRO_HASH_GAIN;
  return static_cast<float>(wave - std::floor(wave));
}

}  // namespace rtype::client
