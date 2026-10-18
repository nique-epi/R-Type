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

/**
 * @brief A random draw outside 0 up to, but not including, 1 was given where a
 * unit draw is expected.
 */
class InvalidRandomUnitException : public GameException {
 public:
  InvalidRandomUnitException();
};

}  // namespace rtype::game
