#include "Kopecks.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include "EconomyConstants.hpp"
#include "SettingsConstants.hpp"

namespace rtype::client {

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

std::string kopecksText(std::uint32_t amount) {
  const std::string digits = std::to_string(amount);
  std::string text;
  for (std::size_t place = 0; place < digits.size(); ++place) {
    if (place > 0 && (digits.size() - place) % KOPECKS_DIGIT_GROUP == 0) {
      text += KOPECKS_GROUP_SEPARATOR;
    }
    text += digits[place];
  }
  return text;
}

}  // namespace rtype::client
