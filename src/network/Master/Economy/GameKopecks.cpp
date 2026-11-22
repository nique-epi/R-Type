#include "GameKopecks.hpp"
#include <algorithm>
#include <cstdint>
#include "GameGain.hpp"
#include "KopecksConstants.hpp"

namespace rtype::network {

std::uint32_t boundedKopecks(std::uint64_t amount) {
  return static_cast<std::uint32_t>(
      std::min<std::uint64_t>(amount, MAXIMUM_KOPECKS));
}

std::uint32_t kopecksFor(GameGain gain) {
  return boundedKopecks(std::uint64_t{gain.bounty} +
                        (gain.score / SCORE_POINTS_PER_KOPECK));
}

}  // namespace rtype::network
