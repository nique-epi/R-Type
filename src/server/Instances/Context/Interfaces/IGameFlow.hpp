#pragma once

#include <cstdint>
#include "GameState.hpp"

namespace rtype::server {

/**
 * @brief What a game shows of the way it unfolds, whatever the rules of the
 * game: the module that carries them out provides it as a service, and the
 * GameInstance answers through it. What happens to the players goes through
 * SessionHooks instead, so any module can follow it.
 */
class IGameFlow {
 public:
  virtual ~IGameFlow() = default;

  [[nodiscard]] virtual GameState state() const = 0;

  /**
   * @returns How many ticks were simulated since the game started: 0 while it
   * waits for players, and again once it waits after a game.
   */
  [[nodiscard]] virtual std::uint32_t tick() const = 0;

  /**
   * @brief Ends the game in progress. Does nothing in any other state.
   */
  virtual void finish() = 0;
};

}  // namespace rtype::server
