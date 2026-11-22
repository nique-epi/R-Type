#pragma once

#include <string>
#include <string_view>
#include "GameRules.hpp"
#include "Language.hpp"

namespace rtype::client {

/** @returns @p text with its ASCII letters in capitals, as the interface
 * writes every word. */
[[nodiscard]] std::string upperCased(std::string_view text);

/** @returns The name of @p difficulty in @p language: a difficulty the
 * client knows by its translation, any other in capitals. */
[[nodiscard]] std::string difficultyNameOf(std::string_view difficulty,
                                           Language language);

/** @returns The rules of a game in words, in @p language, as the panel of the
 * games and the waiting room write them: its difficulty, its lives, and its
 * friendly fire. */
[[nodiscard]] std::string rulesLineOf(const network::GameRules& rules,
                                      Language language);

}  // namespace rtype::client
