#pragma once

#include <cstdint>
#include "ScriptedSpawn.hpp"

namespace rtype::game {

/**
 * @brief The game the scripts create and destroy bodies in. A script runs
 * while the bodies are being visited, so both are carried out once that visit
 * is over, before the next tick.
 */
class IScriptWorld {
 public:
  virtual ~IScriptWorld() = default;

  /**
   * @returns The network id the body will have.
   * @throws engine::ScriptException, saying why, for a sheet that does not
   * exist or that a ship is made from, or past the bodies of a tick.
   */
  [[nodiscard]] virtual std::uint32_t spawn(const ScriptedSpawn& spawn) = 0;

  /**
   * @brief Removes the body of @p networkId: an id no body has does nothing,
   * and the ship of a player is never removed.
   * @throws engine::ScriptException past the removals of a tick.
   */
  virtual void destroy(std::uint32_t networkId) = 0;
};

}  // namespace rtype::game
