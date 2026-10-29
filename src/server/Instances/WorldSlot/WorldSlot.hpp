#pragma once

#include <memory>
#include "World.hpp"

namespace rtype::server {

/**
 * @brief The world of a game, which a new one replaces each time the game
 * waits for players again.
 *
 * It always holds a world: the constructor builds the first one. Not
 * thread-safe: use it from the simulation thread only.
 */
class WorldSlot {
 public:
  WorldSlot();

  /**
   * @returns The world of the game. A call to renew() destroys it, so do not
   * keep the reference across one.
   */
  [[nodiscard]] game::World& current();

  /**
   * @brief Destroys the world and builds a new one in its place.
   * @returns The new world. Whoever works on the previous one rebuilds its
   * parts from it afterwards.
   */
  game::World& renew();

 private:
  std::unique_ptr<game::World> world_;
};

}  // namespace rtype::server
