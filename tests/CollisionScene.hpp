#pragma once

#include "Collision.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "EntityType.hpp"
#include "EventBus.hpp"

/**
 * @brief An entity registry, its components and a bus, with a way to create
 * entities that carry a position, a collision box and a type.
 */
class CollisionScene {
 public:
  CollisionScene();

  rtype::engine::Entity spawn(rtype::game::EntityType type, float centerX,
                              float centerY);
  rtype::engine::Entity spawnWithoutCollisionBox(rtype::game::EntityType type,
                                                 float centerX, float centerY);

  rtype::engine::EntityRegistry entities;
  rtype::engine::ComponentRegistry components;
  rtype::engine::EventBus events;
};

/**
 * @returns true when the collision names exactly these two entities, in either
 * order.
 */
bool isPair(const rtype::engine::Collision& collision,
            rtype::engine::Entity first, rtype::engine::Entity second);
