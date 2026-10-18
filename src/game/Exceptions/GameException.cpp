#include "GameException.hpp"
#include <stdexcept>
#include <string>

namespace rtype::game {

GameException::GameException(const std::string& message)
    : std::runtime_error(message) {}

InvalidPlayerNumberException::InvalidPlayerNumberException()
    : GameException(
          "The player number must be between 1 and the number of players") {}

}  // namespace rtype::game
