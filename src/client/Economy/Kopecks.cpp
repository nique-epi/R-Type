#include "Kopecks.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include "EconomyConstants.hpp"

namespace rtype::client {

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
