#include "PlayfieldExitSystemFixture.hpp"
#include <vector>
#include "Boss.hpp"
#include "CollisionBox.hpp"
#include "Direction.hpp"
#include "Entity.hpp"
#include "EntityDestruction.hpp"
#include "EntityRegistry.hpp"
#include "EntityType.hpp"
#include "Intent.hpp"
#include "NetworkIdentity.hpp"
#include "PlayfieldExitSystemTestConstants.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

PlayfieldExitSystemFixture::PlayfieldExitSystemFixture() {
  events_.subscribe<EntityDestruction>(
      [this](const EntityDestruction& event) { received_.push_back(event); });
}

engine::Entity PlayfieldExitSystemFixture::add(EntityType type,
                                               float positionX) {
  if (type == EntityType::PlayerMissile) {
    return addMissileWithBox(type, positionX, PLAYER_MISSILE_EXIT_BOX);
  }
  if (type == EntityType::EnemyMissile) {
    return addMissileWithBox(type, positionX, ENEMY_MISSILE_EXIT_BOX);
  }
  return addWithoutBox(type, positionX);
}

engine::Entity PlayfieldExitSystemFixture::addMissileWithBox(EntityType type,
                                                             float positionX,
                                                             CollisionBox box) {
  const engine::Entity missile = addWithoutBox(type, positionX);
  components_.add(missile, box);
  return missile;
}

engine::Entity PlayfieldExitSystemFixture::addWithoutBox(EntityType type,
                                                         float positionX) {
  const engine::Entity entity = entities_.create();
  components_.add(entity, Position{.x = positionX, .y = EXIT_ROW_Y});
  components_.add(entity, type);
  components_.add(entity, NetworkIdentity{.networkId = EXITING_NETWORK_ID});
  return entity;
}

engine::Entity PlayfieldExitSystemFixture::addSteeredMissile(
    Position position, Direction heading) {
  const engine::Entity missile = entities_.create();
  components_.add(missile, position);
  components_.add(missile, EntityType::EnemyMissile);
  components_.add(missile, NetworkIdentity{.networkId = EXITING_NETWORK_ID});
  components_.add(missile, ENEMY_MISSILE_EXIT_BOX);
  components_.add(missile, Intent{.direction = heading, .steered = true});
  return missile;
}

engine::Entity PlayfieldExitSystemFixture::addObstacle(float positionX,
                                                       CollisionBox box) {
  const engine::Entity obstacle =
      addWithoutBox(EntityType::Obstacle, positionX);
  components_.add(obstacle, box);
  return obstacle;
}

engine::Entity PlayfieldExitSystemFixture::addBoss(float positionX) {
  const engine::Entity boss = add(EntityType::Enemy, positionX);
  components_.add(boss, Boss{});
  return boss;
}

void PlayfieldExitSystemFixture::updateAndDispatch() {
  exit_.update(components_, engine::SIMULATION_TICK_DURATION);
  events_.dispatch();
}

const engine::EntityRegistry& PlayfieldExitSystemFixture::entities() const {
  return entities_;
}

const std::vector<EntityDestruction>& PlayfieldExitSystemFixture::received()
    const {
  return received_;
}

}  // namespace rtype::game::testing
