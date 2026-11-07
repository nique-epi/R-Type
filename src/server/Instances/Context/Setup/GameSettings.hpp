#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include "EntitySheets.hpp"
#include "GameRuleConstants.hpp"
#include "ServerConstants.hpp"
#include "StateEncoding.hpp"
#include "WavePlan.hpp"

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
  /** @brief The name the results of the game are filed under; waves holds the
   * waves of that difficulty, not its name. */
  std::string difficulty{identifiers::DEFAULT_DIFFICULTY};
  /** @brief The lives each player starts the game with, and gets back when a
   * new world is built. */
  std::uint8_t lives{identifiers::DEFAULT_LIVES};
  /** @brief Whether a missile of a player costs another player a life. */
  bool friendlyFire{false};
  /** @brief Whether the game is played by the rules the game ships with (see
   * vanillaBreaches()): only the results of a vanilla game may leave this
   * server. */
  bool vanilla{false};
};

}  // namespace rtype::server
