#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include "EntitySheets.hpp"
#include "GameRules.hpp"
#include "ServerConstants.hpp"
#include "StateEncoding.hpp"
#include "WavePlan.hpp"

namespace rtype::game {

class BehaviourScripts;

}  // namespace rtype::game

namespace rtype::server {

/**
 * @brief What one game is set up with: the seed its random draws start from,
 * the waves of its difficulty, the entity sheets its entities are made from,
 * the world states it sends per second and how it writes them, the ticks it
 * holds each input, its rules (the name of its difficulty, the lives of each
 * player, friendly fire) and whether it is vanilla.
 */
struct GameSettings {
  std::uint32_t seed{};
  WavePlan waves;
  /** @brief The sheets of the types of the game, which outlive it: the ship
   * of a player is SHIP_SHEET_NAME, and the waves name enemies among them. */
  std::reference_wrapper<const game::EntitySheets> types;
  std::size_t statesPerSecond{DEFAULT_WORLD_STATES_PER_SECOND};
  StateEncoding stateEncoding{DEFAULT_STATE_ENCODING};
  std::size_t inputDelayTicks{DEFAULT_INPUT_DELAY_TICKS};
  /** @brief The rules its creator chose: the difficulty the results are filed
   * under (waves holds the waves of that difficulty, not its name), the lives
   * each player starts with and gets back with each new world, and friendly
   * fire. */
  network::GameRules rules{network::defaultGameRules()};
  /** @brief Whether the game is played by the rules the game ships with (see
   * vanillaBreaches()): only the results of a vanilla game may leave this
   * server. */
  bool vanilla{false};
  /** @brief Whether the game skips its waves and brings its boss in at
   * once; a game started so is never vanilla. */
  bool startsAtBoss{false};
  /** @brief The scripts its bodies run on, which outlive it; nullptr in a game
   * built without any. */
  game::BehaviourScripts* scripts{nullptr};
};

}  // namespace rtype::server
