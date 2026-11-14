#pragma once

#include <span>
#include "Collision.hpp"
#include "CollisionBox.hpp"
#include "CollisionFilter.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "EntityType.hpp"
#include "EventBus.hpp"

/**
 * @brief An entity registry, its components and a bus, with a way to create
 * entities that carry a position, a collision box, a type and the collision
 * filter the shipped sheets give that type (see shippedFilterOf()).
 */
class CollisionScene {
 public:
  CollisionScene();

  rtype::engine::Entity spawn(rtype::game::EntityType type, float centerX,
                              float centerY);
  rtype::engine::Entity spawnWithoutCollisionBox(rtype::game::EntityType type,
                                                 float centerX, float centerY);
  /** @returns An entity of @p type at the origin, with the collision box of
   * spawn() and @p filter in place of the one of the shipped sheets. */
  rtype::engine::Entity spawnWith(rtype::game::EntityType type,
                                  rtype::engine::CollisionFilter filter);

  /** @returns An entity of @p type at the origin that collides with @p parts
   * in place of the collision box of spawn(). */
  rtype::engine::Entity spawnInParts(
      rtype::game::EntityType type,
      std::span<const rtype::game::CollisionBox> parts);

  rtype::engine::EntityRegistry entities;
  rtype::engine::ComponentRegistry components;
  rtype::engine::EventBus events;
};

/** @returns The collision filter the shipped sheets give a body of side
 * @p type; OBSTACLE_FILTER for an obstacle, a side no shipped sheet has. */
rtype::engine::CollisionFilter shippedFilterOf(rtype::game::EntityType type);

/**
 * @returns true when the collision names exactly these two entities, in either
 * order.
 */
bool isPair(const rtype::engine::Collision& collision,
            rtype::engine::Entity first, rtype::engine::Entity second);
