#pragma once

#include <cstdint>
#include <optional>
#include "ComponentRegistry.hpp"
#include "EnemyAction.hpp"
#include "Entity.hpp"
#include "EntityAppearance.hpp"
#include "EntityRegistry.hpp"
#include "PaintedBeamAnnouncer.hpp"
#include "PaintedBeams.hpp"

namespace rtype::client::testing {

/** @brief An enemy whose sheet asks the client to paint its hurt beams, of
 * network id BEAMING_ENEMY_ID, and the timeline of its actions. */
class PaintedBeamsScene {
 public:
  PaintedBeamsScene();

  /** @brief Announces @p phase of the beam attack of the enemy @p entityId,
   * the client estimating the server at @p serverTick. */
  void announce(std::uint32_t entityId, network::EnemyActionPhase phase,
                std::optional<double> serverTick);

  /** @brief Shows the actions due at @p renderTick, the tick the client
   * draws. */
  void drawTick(std::optional<double> renderTick);

  /** @returns The PaintedBeams of the enemy. */
  [[nodiscard]] const PaintedBeams& beams();

 private:
  EntityAppearance appearance_;
  engine::EntityRegistry entities_;
  engine::ComponentRegistry components_{entities_};
  engine::Entity enemy_{};
  PaintedBeamAnnouncer announcer_{components_};
};

}  // namespace rtype::client::testing
