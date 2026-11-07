#pragma once

#include <gtest/gtest.h>
#include <vector>
#include "Collision.hpp"
#include "CollisionScene.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "MissileHit.hpp"
#include "ShipContact.hpp"

namespace rtype::game::testing {

/** @brief The events the collision rules published, oldest first. */
struct Recorded {
  std::vector<MissileHit> missileHits;
  std::vector<ShipContact> shipContacts;
};

/** @brief Records in @p recorded every MissileHit and ShipContact of the bus
 * of @p scene; @p recorded must outlive the dispatches. */
void record(CollisionScene& scene, Recorded& recorded);

/** @brief Checks that a Collision of an entity of @p firstType and one of
 * @p secondType gives no event. */
void expectNoEventFor(EntityType firstType, EntityType secondType);

/** @brief The tests run with a Collision naming its entities in one order,
 * then in the other. */
class CollisionRulesInAnyOrder : public ::testing::TestWithParam<bool> {
 protected:
  /** @returns A Collision of @p entity and @p other, in the order of the
   * parameter. */
  [[nodiscard]] static engine::Collision collisionOf(engine::Entity entity,
                                                     engine::Entity other);
};

}  // namespace rtype::game::testing
