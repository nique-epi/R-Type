#pragma once

#include <stdexcept>
#include <string>

namespace rtype::game {

/**
 * @brief Root of every error raised by the game library.
 */
class GameException : public std::runtime_error {
 public:
  explicit GameException(const std::string& message);
};

/**
 * @brief A player number outside 1 to the number of players of the game was
 * given.
 */
class InvalidPlayerNumberException : public GameException {
 public:
  InvalidPlayerNumberException();
};

}  // namespace rtype::game
