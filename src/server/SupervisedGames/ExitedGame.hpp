#pragma once

#include <cstdint>
#include "GameLifecycle.hpp"

namespace rtype::server {

/** @brief A game process that ended, and how, as the registry hands it back. */
struct ExitedGame {
  std::uint32_t number{};
  GameLifecycle lifecycle{GameLifecycle::Starting};
  std::uint16_t port{};
  std::uint64_t linkToken{};
  int exitCode{};
  /** @brief Whether it ended other than by itself: a code other than 0, or a
   * code 0 before it ever reported. */
  bool crashed{false};
};

}  // namespace rtype::server
