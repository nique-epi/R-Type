#include "LagCompensatedHitSystemShipsFixture.hpp"
#include <cstddef>
#include <vector>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "HurtZone.hpp"
#include "LagCompensatedHitSystemShipsTestConstants.hpp"
#include "MissileHit.hpp"
#include "Position.hpp"
#include "ShipContact.hpp"
#include "TimeConstants.hpp"
#include "ViewDelay.hpp"

namespace rtype::game::testing {

LagCompensatedHitSystemShipsFixture::LagCompensatedHitSystemShipsFixture() {
  static_cast<void>(events_.subscribe<MissileHit>(
      [this](const MissileHit& hit) { hits_.push_back(hit); }));
  static_cast<void>(events_.subscribe<ShipContact>(
      [this](const ShipContact& contact) { contacts_.push_back(contact); }));
}

engine::Entity LagCompensatedHitSystemShipsFixture::shipAt(
    Position position, std::size_t ticksBehind) {
  const engine::Entity ship = entities_.create();
  components_.add(ship, position);
  components_.add(ship, EntityType::Player);
  components_.add(ship, SHIP_BOX);
  components_.add(ship, ViewDelay{.ticks = ticksBehind});
  return ship;
}

engine::Entity LagCompensatedHitSystemShipsFixture::enemyMissileAt(
    Position position) {
  const engine::Entity missile = entities_.create();
  components_.add(missile, position);
  components_.add(missile, EntityType::EnemyMissile);
  components_.add(missile, ENEMY_MISSILE_BOX);
  return missile;
}

engine::Entity LagCompensatedHitSystemShipsFixture::hurtZoneAt(
    Position position) {
  const engine::Entity enemy = entities_.create();
  components_.add(enemy, position);
  components_.add(enemy, EntityType::Enemy);
  components_.add(enemy, HurtZone{.boxes = {ALWAYS_HURTING_BOX}});
  return enemy;
}

void LagCompensatedHitSystemShipsFixture::move(engine::Entity entity,
                                               Position position) {
  *components_.get<Position>(entity) = position;
}

void LagCompensatedHitSystemShipsFixture::runTicks(std::size_t count) {
  for (std::size_t tick = 0; tick < count; ++tick) {
    system_.update(components_, engine::SIMULATION_TICK_DURATION);
    events_.dispatch();
  }
}

const std::vector<MissileHit>& LagCompensatedHitSystemShipsFixture::hits()
    const {
  return hits_;
}

const std::vector<ShipContact>& LagCompensatedHitSystemShipsFixture::contacts()
    const {
  return contacts_;
}

engine::ComponentRegistry& LagCompensatedHitSystemShipsFixture::components() {
  return components_;
}

}  // namespace rtype::game::testing
