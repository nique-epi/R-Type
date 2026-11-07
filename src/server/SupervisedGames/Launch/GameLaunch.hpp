#pragma once

#include <cstdint>
#include "GameRules.hpp"

namespace rtype::server {

/** @brief What differs from one game process to the next. */
struct GameLaunch {
  std::uint32_t number{};
  /** @brief The port the game listens on. */
  std::uint16_t port{};
  /** @brief The port of the supervisor, which the game talks to. */
  std::uint16_t supervisorPort{};
  std::uint64_t linkToken{};
  /** @brief The rules the game is played by, as its creator chose them. */
  network::GameRules rules{network::defaultGameRules()};
};

}  // namespace rtype::server
