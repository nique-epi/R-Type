#pragma once

#include <cstdint>
#include <functional>
#include "ConnectedPlayer.hpp"
#include "GameContext.hpp"
#include "World.hpp"

namespace rtype::server {

/** @brief What the MatchSystems of one world are built from. */
struct MatchSystemsParts {
  /** @brief The game the world is played in; it must outlive the systems. */
  std::reference_wrapper<GameContext> game;
  /** @brief The world the systems run on; it must outlive them. */
  std::reference_wrapper<game::World> world;
  /** @brief The seed the waves of this world draw from. */
  std::uint32_t waveSeed{0};
  /** @brief What is called when a player gets a new ship. Must not be
   * empty. */
  std::function<void(ConnectedPlayer&)> respawn;
};

}  // namespace rtype::server
