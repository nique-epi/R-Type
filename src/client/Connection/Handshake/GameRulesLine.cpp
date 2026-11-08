#include "GameRulesLine.hpp"
#include <algorithm>
#include <cctype>
#include <string>
#include <string_view>
#include "GameRules.hpp"
#include "GameRulesLineConstants.hpp"

namespace rtype::client {

std::string upperCased(std::string_view text) {
  std::string upper(text);
  std::ranges::transform(upper, upper.begin(), [](char character) noexcept {
    return static_cast<char>(
        std::toupper(static_cast<unsigned char>(character)));
  });
  return upper;
}

std::string rulesLineOf(const network::GameRules& rules) {
  return upperCased(rules.difficulty) + RULES_SEPARATOR +
         std::to_string(rules.lives) +
         (rules.lives == 1 ? ONE_LIFE_WORD : LIVES_WORD) + RULES_SEPARATOR +
         (rules.friendlyFire ? FRIENDLY_FIRE_ON_WORDS
                             : FRIENDLY_FIRE_OFF_WORDS);
}

}  // namespace rtype::client
