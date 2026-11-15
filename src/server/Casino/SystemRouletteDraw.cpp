#include "SystemRouletteDraw.hpp"
#include <cstdint>
#include <random>
#include "ProtocolConstants.hpp"

namespace rtype::server {

std::uint8_t SystemRouletteDraw::drawNumber() {
  std::uniform_int_distribution<int> wheel{0, network::HIGHEST_ROULETTE_NUMBER};
  return static_cast<std::uint8_t>(wheel(random_));
}

}  // namespace rtype::server
