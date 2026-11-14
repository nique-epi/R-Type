#include "CollisionScene.hpp"
#include <span>
#include "Collision.hpp"
#include "CollisionBox.hpp"
#include "CollisionFilter.hpp"
#include "CollisionTestConstants.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "HitboxParts.hpp"
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
  components.add(entity, shippedFilterOf(type));
  return entity;
}

Entity CollisionScene::spawnWith(EntityType type,
                                 rtype::engine::CollisionFilter filter) {
  const Entity entity = spawn(type, 0.0F, 0.0F);
  *components.get<rtype::engine::CollisionFilter>(entity) = filter;
  return entity;
}

Entity CollisionScene::spawnInParts(EntityType type,
                                    std::span<const CollisionBox> parts) {
  const Entity entity = spawn(type, 0.0F, 0.0F);
  components.add(
      entity, rtype::game::HitboxParts{.boxes = {parts.begin(), parts.end()}});
  return entity;
}

rtype::engine::CollisionFilter shippedFilterOf(EntityType type) {
  switch (type) {
    case EntityType::Player:
      return PLAYER_FILTER;
    case EntityType::Enemy:
      return ENEMY_FILTER;
    case EntityType::PlayerMissile:
      return PLAYER_MISSILE_FILTER;
    case EntityType::EnemyMissile:
      return ENEMY_MISSILE_FILTER;
    case EntityType::LookItem:
      return LOOK_ITEM_FILTER;
    case EntityType::Obstacle:
      return OBSTACLE_FILTER;
  }
  return rtype::engine::CollisionFilter{};
}

bool isPair(const rtype::engine::Collision& collision, Entity first,
            Entity second) {
  return (collision.first == first && collision.second == second) ||
         (collision.first == second && collision.second == first);
}
