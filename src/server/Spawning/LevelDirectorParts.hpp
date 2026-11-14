#pragma once

#include <functional>
#include <string>
#include "BehaviourScripts.hpp"
#include "BossEntranceParts.hpp"
#include "EntitySheets.hpp"
#include "PercentConstants.hpp"

namespace rtype::server {

/** @brief What a LevelDirector is built from. */
struct LevelDirectorParts {
  /** @brief The world the level is played in, the players told about its
   * bosses, and the bus its changes, the removals of a restart and the
   * arrivals of its bosses are published on; they must outlive the object. */
  BossEntranceParts entrance;
  /** @brief The scripts that registered the level; they must outlive the
   * object. */
  std::reference_wrapper<game::BehaviourScripts> scripts;
  /** @brief The name the level was registered under. */
  std::string level;
  /** @brief The sheets the bosses of the level are named among; they must
   * outlive the object. */
  std::reference_wrapper<const game::EntitySheets> types;
  /** @brief The health of the bosses in percent of their sheet, for the
   * difficulty (WavePlan::bossHealthPercent). */
  int bossHealthPercent{game::WHOLE_PERCENT};
};

}  // namespace rtype::server
