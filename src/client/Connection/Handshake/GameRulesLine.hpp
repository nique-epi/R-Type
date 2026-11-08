#pragma once

#include <string>
#include <string_view>
#include "GameRules.hpp"

namespace rtype::client {

/** @returns @p text with its ASCII letters in capitals, as the interface
 * writes every word. */
[[nodiscard]] std::string upperCased(std::string_view text);

/** @returns The rules of a game in words, as the panel of the games and the
 * waiting room write them: its difficulty in capitals, its lives, and its
 * friendly fire. */
[[nodiscard]] std::string rulesLineOf(const network::GameRules& rules);

}  // namespace rtype::client
