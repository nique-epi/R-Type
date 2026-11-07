#pragma once

#include <cstdint>
#include <vector>
#include "CollisionScene.hpp"
#include "Entity.hpp"
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

/** @returns A player missile of @p scene at the origin, fired by the player
 * numbered @p owner. */
rtype::engine::Entity spawnMissileOf(CollisionScene& scene, std::uint8_t owner);

/** @returns The ship of a player in @p scene at the origin, flown by the
 * player numbered @p pilot. */
rtype::engine::Entity spawnShipFlownBy(CollisionScene& scene,
                                       std::uint8_t pilot);

/** @brief Expects that a Collision of an entity of @p firstType with one of
 * @p secondType publishes nothing, friendly fire off. */
void expectNoEventFor(rtype::game::EntityType firstType,
                      rtype::game::EntityType secondType);
