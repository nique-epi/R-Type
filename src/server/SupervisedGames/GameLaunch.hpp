#pragma once

#include <cstdint>

namespace rtype::server {

/** @brief What differs from one game process to the next. */
struct GameLaunch {
  std::uint32_t number{};
  /** @brief The port the game listens on. */
  std::uint16_t port{};
  /** @brief The port of the supervisor, which the game talks to. */
  std::uint16_t supervisorPort{};
  std::uint64_t linkToken{};
};

}  // namespace rtype::server
