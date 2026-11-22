#pragma once

#include <optional>
#include "ComponentRegistry.hpp"
#include "EnemyAction.hpp"

namespace rtype::client {

/**
 * @brief Records each action announced to an enemy whose beams the client
 * paints on its PaintedBeams, stamped with the server tick the client
 * estimates when it arrives; showPaintedBeams() then shows it when the tick
 * the client draws reaches that stamp.
 *
 * An EnemyAction carries no tick, and the client draws the enemies behind
 * the server (InterpolationClock): stamped, an action is shown on the
 * timeline the server tests the ships against what their players saw.
 * Guaranteed messages arrive in order, so the actions of an enemy are
 * recorded in the order of their ticks. An enemy the client does not have, or
 * without PaintedBeams, records nothing. Without an estimate yet, an action
 * is stamped 0, so it shows at once. The registry must outlive the announcer.
 */
class PaintedBeamAnnouncer {
 public:
  explicit PaintedBeamAnnouncer(engine::ComponentRegistry& components);

  /** @brief Stamps the actions announced from now on with @p serverTick, the
   * server tick the client estimates; nothing without an estimate. */
  void followServerTick(std::optional<double> serverTick);

  /** @brief Records @p action; see the class. */
  void apply(const network::EnemyAction& action);

 private:
  engine::ComponentRegistry* components_;
  std::optional<double> serverTick_;
};

}  // namespace rtype::client
