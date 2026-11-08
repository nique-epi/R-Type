#include "LagCompensatedHitSystemFixture.hpp"
#include <cstddef>
#include <memory>
#include <utility>
#include <vector>
#include "CollisionBox.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "LagCompensatedHitSystemTestConstants.hpp"
#include "MissileHit.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"
#include "ViewDelay.hpp"

namespace rtype::game::testing {

LagCompensatedHitSystemFixture::LagCompensatedHitSystemFixture() {
  static_cast<void>(events_.subscribe<MissileHit>(
      [this](const MissileHit& hit) { hits_.push_back(hit); }));
}

engine::Entity LagCompensatedHitSystemFixture::enemyAt(Position position) {
  const engine::Entity enemy = entities_->create();
  components_->add(enemy, position);
  components_->add(enemy, EntityType::Enemy);
  components_->add(enemy, ENEMY_BOX);
  return enemy;
}

engine::Entity LagCompensatedHitSystemFixture::missileAt(
    Position position, std::size_t ticksBehind) {
  const engine::Entity missile = entities_->create();
  components_->add(missile, position);
  components_->add(missile, EntityType::PlayerMissile);
  components_->add(missile, MISSILE_BOX);
  components_->add(missile, ViewDelay{.ticks = ticksBehind});
  return missile;
}

void LagCompensatedHitSystemFixture::move(engine::Entity entity,
                                          Position position) {
  *components_->get<Position>(entity) = position;
}

void LagCompensatedHitSystemFixture::runTick() {
  system_.update(*components_, engine::SIMULATION_TICK_DURATION);
  events_.dispatch();
}

void LagCompensatedHitSystemFixture::startANewWorld() {
  auto entities = std::make_unique<engine::EntityRegistry>();
  auto components = std::make_unique<engine::ComponentRegistry>(*entities);
  components_ = std::move(components);
  entities_ = std::move(entities);
}

void LagCompensatedHitSystemFixture::runTicks(std::size_t count) {
  for (std::size_t tick = 0; tick < count; ++tick) {
    runTick();
  }
}

const std::vector<MissileHit>& LagCompensatedHitSystemFixture::hits() const {
  return hits_;
}

engine::ComponentRegistry& LagCompensatedHitSystemFixture::components() {
  return *components_;
}

}  // namespace rtype::game::testing
