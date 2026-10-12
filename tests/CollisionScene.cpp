#include "CollisionScene.hpp"
#include "Collision.hpp"
#include "CollisionBox.hpp"
#include "CollisionTestConstants.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "Position.hpp"

using rtype::engine::Entity;
using rtype::game::CollisionBox;
using rtype::game::EntityType;
using rtype::game::Position;

CollisionScene::CollisionScene() : components(entities) {}

Entity CollisionScene::spawn(EntityType type, float centerX, float centerY) {
  const Entity entity = spawnWithoutCollisionBox(type, centerX, centerY);
  components.add(entity,
                 CollisionBox{.width = SQUARE_SIDE, .height = SQUARE_SIDE});
  return entity;
}

Entity CollisionScene::spawnWithoutCollisionBox(EntityType type, float centerX,
                                                float centerY) {
  const Entity entity = entities.create();
  components.add(entity, Position{.x = centerX, .y = centerY});
  components.add(entity, type);
  return entity;
}

bool isPair(const rtype::engine::Collision& collision, Entity first,
            Entity second) {
  return (collision.first == first && collision.second == second) ||
         (collision.first == second && collision.second == first);
}
