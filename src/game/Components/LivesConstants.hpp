#pragma once

#include <cstdint>
#include "GameRuleConstants.hpp"

namespace rtype::game {

/**
 * @brief The lives a player starts a game with, and gets back when a new world
 * is built.
 */
constexpr std::uint8_t STARTING_LIVES = identifiers::DEFAULT_LIVES;

}  // namespace rtype::game
