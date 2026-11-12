#include "LagCompensatedHitSystemShotDownFixture.hpp"
#include <cstddef>
#include <vector>
#include "CollisionTestConstants.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "LagCompensatedHitSystemShipsTestConstants.hpp"
#include "LagCompensatedHitSystemTestConstants.hpp"
#include "MissileHit.hpp"
#include "Position.hpp"
#include "Shootable.hpp"
#include "TimeConstants.hpp"
#include "ViewDelay.hpp"

namespace rtype::game::testing {

LagCompensatedHitSystemShotDownFixture::
    LagCompensatedHitSystemShotDownFixture() {
  static_cast<void>(events_.subscribe<MissileHit>(
      [this](const MissileHit& hit) { hits_.push_back(hit); }));
}

engine::Entity LagCompensatedHitSystemShotDownFixture::shotAt(
    Position position, std::size_t ticksBehind) {
  const engine::Entity shot = entities_.create();
  components_.add(shot, position);
  components_.add(shot, EntityType::PlayerMissile);
  components_.add(shot, MISSILE_BOX);
  components_.add(shot, PLAYER_MISSILE_FILTER);
  components_.add(shot, ViewDelay{.ticks = ticksBehind});
  return shot;
}

engine::Entity LagCompensatedHitSystemShotDownFixture::enemyMissileAt(
    Position position) {
  const engine::Entity missile = entities_.create();
  components_.add(missile, position);
  components_.add(missile, EntityType::EnemyMissile);
  components_.add(missile, ENEMY_MISSILE_BOX);
  components_.add(missile, ENEMY_MISSILE_FILTER);
  return missile;
}

engine::Entity
LagCompensatedHitSystemShotDownFixture::shootableMissileThatLeft() {
  const engine::Entity missile = enemyMissileAt(WHERE_THE_PLAYER_SAW_THE_ENEMY);
  components_.add(missile, Shootable{});
  runTicks(1);
  *components_.get<Position>(missile) = WHERE_THE_ENEMY_WENT;
  runTicks(FIVE_TICKS_BEHIND - 1);
  return missile;
}

void LagCompensatedHitSystemShotDownFixture::runTicks(std::size_t count) {
  for (std::size_t tick = 0; tick < count; ++tick) {
    system_.update(components_, engine::SIMULATION_TICK_DURATION);
    events_.dispatch();
  }
}

const std::vector<MissileHit>& LagCompensatedHitSystemShotDownFixture::hits()
    const {
  return hits_;
}

}  // namespace rtype::game::testing
