#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include "EntitySheets.hpp"
#include "ServerConstants.hpp"
#include "WavePlan.hpp"

namespace rtype::server {

/**
 * @brief What one game is set up with: the seed its random draws start from,
 * the waves of its difficulty, the entity sheets its entities are made from,
 * the world states it sends per second, the ticks it holds each input, the
 * name of its difficulty and whether it is vanilla.
 */
struct GameSettings {
  std::uint32_t seed{};
  WavePlan waves;
  /** @brief The sheets of the types of the game, which outlive it: the ship
   * of a player is SHIP_SHEET_NAME, and the waves name enemies among them. */
  std::reference_wrapper<const game::EntitySheets> types;
  std::size_t statesPerSecond{DEFAULT_WORLD_STATES_PER_SECOND};
  std::size_t inputDelayTicks{DEFAULT_INPUT_DELAY_TICKS};
  /** @brief The name the results of the game are filed under; waves holds the
   * waves of that difficulty, not its name. */
  std::string difficulty{DEFAULT_DIFFICULTY};
  /** @brief Whether the game is played by the rules the game ships with (see
   * vanillaBreaches()): only the results of a vanilla game may leave this
   * server. */
  bool vanilla{false};
};

}  // namespace rtype::server
