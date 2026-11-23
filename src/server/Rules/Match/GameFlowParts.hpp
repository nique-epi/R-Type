#pragma once

#include <functional>
#include "GameContext.hpp"
#include "IGameReplication.hpp"
#include "IThreatLevel.hpp"
#include "WorldSlot.hpp"

namespace rtype::server {

/**
 * @brief What a GameFlow is built from.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every reference: the
 * member-init checks, which report its implicit default constructor, see a
 * constructor that cannot be called.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct GameFlowParts {
  /** @brief The context of the game; it must outlive the flow. The flow reads
   * its roster, bus, systems, settings, send function and ready rule. */
  std::reference_wrapper<GameContext> game;
  /** @brief Holds the world of the game, which the flow replaces with each
   * new world; it must outlive the flow. */
  std::reference_wrapper<WorldSlot> world;
  /** @brief Tells the players how the game stands, and gives them their
   * ships; it must outlive the flow. */
  std::reference_wrapper<IGameReplication> replication;
  /** @brief The threat of the game, which the enemies of each world answer;
   * nullptr in a game without the module threat. It must outlive the flow. */
  IThreatLevel* threat{nullptr};
};

}  // namespace rtype::server
