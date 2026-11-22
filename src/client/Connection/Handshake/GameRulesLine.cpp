#include "GameRulesLine.hpp"
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <iterator>
#include <string>
#include <string_view>
#include "GameRuleConstants.hpp"
#include "GameRules.hpp"
#include "GameRulesLineConstants.hpp"
#include "Language.hpp"
#include "TextMark.hpp"
#include "TextMarks.hpp"
#include "Translation.hpp"

namespace rtype::client {

std::string upperCased(std::string_view text) {
  std::string upper(text);
  std::ranges::transform(upper, upper.begin(), [](char character) noexcept {
    return static_cast<char>(
        std::toupper(static_cast<unsigned char>(character)));
  });
  return upper;
}

std::string difficultyNameOf(std::string_view id, Language language) {
  const auto known = std::ranges::find(identifiers::DIFFICULTY_ORDER, id);
  if (known == identifiers::DIFFICULTY_ORDER.end()) {
    return upperCased(id);
  }
  const auto order = static_cast<std::size_t>(
      std::distance(identifiers::DIFFICULTY_ORDER.begin(), known));
  return std::string(DIFFICULTY_NAMES.at(order).in(language));
}

std::string rulesLineOf(const network::GameRules& rules, Language language) {
  const std::string lives = std::to_string(rules.lives);
  const Translation& livesWords =
      rules.lives == 1 ? ONE_LIFE_WORDS : LIVES_WORDS;
  return difficultyNameOf(rules.difficulty, language) + RULES_SEPARATOR +
         withMark(livesWords.in(language),
                  TextMark{.mark = LIVES_MARK, .value = lives}) +
         RULES_SEPARATOR +
         std::string((rules.friendlyFire ? FRIENDLY_FIRE_ON_WORDS
                                         : FRIENDLY_FIRE_OFF_WORDS)
                         .in(language));
}

}  // namespace rtype::client
