#pragma once

#include <cstdint>
#include <string_view>
#include "Position.hpp"

namespace rtype::game {

/**
 * @brief Orders to create and remove bodies, given while the bodies are being
 * visited and carried out by the game once that visit is over, before the
 * next tick. The scripts give them through `behaviours.spawn` and
 * `behaviours.destroy`.
 */
class IEntityOrders {
 public:
  virtual ~IEntityOrders() = default;

  /**
   * @returns The network id the body of the sheet @p type, appearing at
   * @p position, will have.
   * @throws engine::ScriptException, saying why, for a sheet that does not
   * exist or that a ship is made from, or past the bodies of a tick.
   */
  [[nodiscard]] virtual std::uint32_t spawn(std::string_view type,
                                            Position position) = 0;

  /**
   * @brief Removes the body of @p networkId: an id no body has does nothing,
   * and the ship of a player is never removed.
   * @throws engine::ScriptException past the removals of a tick.
   */
  virtual void destroy(std::uint32_t networkId) = 0;
};

}  // namespace rtype::game
