#pragma once

#include "EventBus.hpp"

namespace rtype::server {

/**
 * @brief The bus of the game the process runs, for a module that follows what
 * happens in the game: a module listed after the games module, or a plugin,
 * which reaches the game through this interface and holds none of its code.
 *
 * The games module provides it in its install(). The bus exists from then on
 * and is never replaced, though the game publishes on it only once the games
 * module started. It delivers its events in the outputs stage, on the
 * simulation thread, so a subscriber runs there.
 */
class IGameEvents {
 public:
  virtual ~IGameEvents() = default;

  /** @returns The bus the game publishes on. */
  [[nodiscard]] virtual engine::EventBus& events() = 0;
};

}  // namespace rtype::server
