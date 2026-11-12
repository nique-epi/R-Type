#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "EventBus.hpp"
#include "LagCompensatedHitSystem.hpp"
#include "MissileHit.hpp"
#include "Position.hpp"

namespace rtype::game::testing {

/**
 * @brief A world with one LagCompensatedHitSystem, the missiles of the players
 * it tests and the missiles of the enemies they meet, the MissileHit events it
 * published recorded after each tick.
 */
class LagCompensatedHitSystemShotDownFixture : public ::testing::Test {
 protected:
  LagCompensatedHitSystemShotDownFixture();

  /** @returns A missile of a player at @p position whose player saw the
   * enemies @p ticksBehind ticks behind the present. */
  engine::Entity shotAt(Position position, std::size_t ticksBehind);
  /** @returns A missile of an enemy at @p position, with the box and the
   * filter of the shipped sheet, and no Shootable. */
  engine::Entity enemyMissileAt(Position position);
  /** @returns A Shootable missile of an enemy that stood where
   * WHERE_THE_PLAYER_SAW_THE_ENEMY for one tick, then at
   * WHERE_THE_ENEMY_WENT for FIVE_TICKS_BEHIND - 1 ticks: the ticks have run.
   */
  engine::Entity shootableMissileThatLeft();
  /** @brief Runs @p count ticks of the system, each followed by its events. */
  void runTicks(std::size_t count);
  /** @returns The hits published so far, oldest first. */
  [[nodiscard]] const std::vector<MissileHit>& hits() const;

 private:
  engine::EntityRegistry entities_;
  engine::ComponentRegistry components_{entities_};
  engine::EventBus events_;
  LagCompensatedHitSystem system_{events_};
  std::vector<MissileHit> hits_;
};

}  // namespace rtype::game::testing
