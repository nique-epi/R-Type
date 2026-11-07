#pragma once

#include <vector>
#include "CollisionScene.hpp"
#include "EntityType.hpp"
#include "MissileHit.hpp"
#include "ShipContact.hpp"

/** @brief The events the rules published in a test, in the order published. */
struct RecordedCollisionEvents {
  std::vector<rtype::game::MissileHit> missileHits;
  std::vector<rtype::game::ShipContact> shipContacts;
};

/** @brief Subscribes @p recorded to the MissileHit and ShipContact events of
 * @p scene; both must outlive the bus. */
void record(CollisionScene& scene, RecordedCollisionEvents& recorded);

/** @brief Expects that a Collision of an entity of @p firstType with one of
 * @p secondType publishes nothing, friendly fire off. */
void expectNoEventFor(rtype::game::EntityType firstType,
                      rtype::game::EntityType secondType);
