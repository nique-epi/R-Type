#pragma once

#include <cstddef>
#include <functional>
#include <string>
#include "BehaviourScripts.hpp"
#include "EventBus.hpp"
#include "IEntityOrders.hpp"
#include "ServerConstants.hpp"
#include "World.hpp"

namespace rtype::server {

/** @brief What a LevelDirector is built from. */
struct LevelDirectorParts {
  /** @brief The world the level is played in; it must outlive the object. */
  std::reference_wrapper<game::World> world;
  /** @brief Where the changes of the level and the removals of a restart are
   * published; it must outlive the object. */
  std::reference_wrapper<engine::EventBus> events;
  /** @brief What creates the enemies the level places, in that world; it
   * must outlive the object. */
  std::reference_wrapper<game::IEntityOrders> orders;
  /** @brief The scripts that registered the level; they must outlive the
   * object. */
  std::reference_wrapper<game::BehaviourScripts> scripts;
  /** @brief The name the level was registered under. */
  std::string level;
  /** @brief The stage the level starts at, from FIRST_LEVEL_STAGE. */
  std::size_t startingStage{FIRST_LEVEL_STAGE};
};

}  // namespace rtype::server
