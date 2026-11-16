#include "GameKopecks.hpp"
#include <algorithm>
#include <cstdint>
#include "KopecksConstants.hpp"

namespace rtype::network {

std::uint32_t boundedKopecks(std::uint64_t amount) {
  return static_cast<std::uint32_t>(
      std::min<std::uint64_t>(amount, MAXIMUM_KOPECKS));
}

std::uint32_t kopecksFor(std::uint32_t score) {
  return boundedKopecks(score / SCORE_POINTS_PER_KOPECK);
}

std::uint32_t withVictoryBonus(std::uint32_t kopecks) {
  const std::uint64_t earned = kopecks;
  return boundedKopecks(
      earned + ((earned * VICTORY_BONUS_PERCENT) / WHOLE_PERCENT_OF_A_GAIN));
}

}  // namespace rtype::network
