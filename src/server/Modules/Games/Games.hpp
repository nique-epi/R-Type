#pragma once

#include <memory>
#include <span>
#include <vector>
#include "GameInstance.hpp"

namespace rtype::server {

/**
 * @brief The games the server runs, in the order they were created, and the
 * rule that decides which one a new player joins: always the first.
 *
 * The games module owns it and provides it as a service, so the lobby asks
 * here where a player goes instead of knowing the rule itself. It starts
 * empty: the games module adds the first game when it starts.
 */
class Games {
 public:
  /** @brief Adds @p game after the ones already there. */
  void add(std::unique_ptr<GameInstance> game);

  /**
   * @returns The game every new player joins: the first one.
   * @throws NoJoinableGameException when no game was added yet.
   */
  [[nodiscard]] GameInstance& joinable() const;

  /** @returns Every game, the joinable one first. */
  [[nodiscard]] std::span<const std::unique_ptr<GameInstance>> all() const;

 private:
  std::vector<std::unique_ptr<GameInstance>> games_;
};

}  // namespace rtype::server
