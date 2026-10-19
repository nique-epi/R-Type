#include "GameException.hpp"
#include <stdexcept>
#include <string>

namespace rtype::game {

GameException::GameException(const std::string& message)
    : std::runtime_error(message) {}

InvalidPlayerNumberException::InvalidPlayerNumberException()
    : GameException(
          "The player number must be between 1 and the number of players") {}

InvalidRandomUnitException::InvalidRandomUnitException()
    : GameException("The random draw must be at least 0 and under 1") {}

}  // namespace rtype::game
