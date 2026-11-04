#include "HurtZoneSystemFixture.hpp"
#include <utility>
#include <vector>
#include "ActionKey.hpp"
#include "CollisionScene.hpp"
#include "CurrentAction.hpp"
#include "EntityType.hpp"
#include "HurtZone.hpp"
#include "HurtZoneSystemTestConstants.hpp"
#include "ShipContact.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

HurtZoneSystemFixture::HurtZoneSystemFixture() : system_(scene_.events) {
  scene_.events.subscribe<ShipContact>(
      [this](const ShipContact& contact) { contacts_.push_back(contact); });
}

void HurtZoneSystemFixture::addEnemy(HurtZone zone) {
  enemy_ = scene_.spawn(EntityType::Enemy, ENEMY_X, ENEMY_Y);
  scene_.components.add(enemy_, std::move(zone));
  scene_.components.add(enemy_, CurrentAction{});
}

void HurtZoneSystemFixture::addShip(float centerX) {
  static_cast<void>(scene_.spawn(EntityType::Player, centerX, ENEMY_Y));
}

void HurtZoneSystemFixture::enterAction(ActionKey action) {
  scene_.components.get<CurrentAction>(enemy_)->key = action;
}

const std::vector<ShipContact>& HurtZoneSystemFixture::run() {
  system_.update(scene_.components, engine::SIMULATION_TICK_DURATION);
  scene_.events.dispatch();
  return contacts_;
}

}  // namespace rtype::game::testing
