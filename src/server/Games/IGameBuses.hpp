#pragma once

#include <memory>
#include <span>
#include "EventBus.hpp"

namespace rtype::server {

/**
 * @brief The bus of each game the server hosts, for a module of the program
 * that follows what happens in every game: a module listed after the games
 * module, or a plugin, which reaches the games through this interface and
 * holds none of their code.
 *
 * The games module provides it in its install(). The list is empty until the
 * games module started, and it does not change after: the games are never
 * replaced. Each bus delivers its events in the outputs stage, on the
 * simulation thread, so a subscriber runs there.
 */
class IGameBuses {
 public:
  virtual ~IGameBuses() = default;

  /** @returns The bus of each game, in the order of the games. */
  [[nodiscard]] virtual std::span<const std::unique_ptr<engine::EventBus>>
  buses() const = 0;
};

}  // namespace rtype::server
