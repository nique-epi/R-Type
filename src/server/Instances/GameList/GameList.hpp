#pragma once

#include <memory>
#include <span>
#include <vector>
#include "GameInstance.hpp"

namespace rtype::server {

/**
 * @brief The games the server runs, in the order they were added, and the
 * rule that says which one a new player joins: the first one.
 *
 * The games module provides it during its install() and fills it in its
 * start(); the lobby seats every new player in joinable(). Not thread-safe:
 * only the simulation thread reads it while the server runs.
 */
class GameList {
 public:
  /** @brief Lists @p game after the games already listed. */
  void add(std::unique_ptr<GameInstance> game);

  /**
   * @returns The game every new player joins: the first one listed.
   * @throws NoGameException when no game is listed.
   */
  [[nodiscard]] GameInstance& joinable() const;

  /** @returns The games, in the order they were added. */
  [[nodiscard]] std::span<const std::unique_ptr<GameInstance>> all() const;

  /** @brief Destroys every game, the last added first. */
  void clear();

 private:
  std::vector<std::unique_ptr<GameInstance>> games_;
};

}  // namespace rtype::server
