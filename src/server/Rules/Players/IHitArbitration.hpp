#pragma once

#include <cstdint>
#include "PlayerRoster.hpp"
#include "World.hpp"

namespace rtype::server {

/**
 * @brief Decides, for a mode of play, where in the past each hit of a game is
 * tested: what gets a ViewDelay at each tick, for LagCompensatedHitSystem to
 * test it where its player saw the enemies. A player who has a ping sees its
 * own ship ahead of the server and the enemies behind it, so a hit tested in
 * the present of the server is not the one the player saw.
 */
class IHitArbitration {
 public:
  virtual ~IHitArbitration() = default;

  /** @brief Gives the entities of @p world that this mode tests in the past
   * the ViewDelay of their players of @p roster at tick @p tick. Called before
   * the systems of each tick of a game in progress. */
  virtual void giveViews(game::World& world, PlayerRoster& roster,
                         std::uint32_t tick) const = 0;
};

}  // namespace rtype::server
