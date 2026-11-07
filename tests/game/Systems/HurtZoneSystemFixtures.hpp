#pragma once

#include <vector>
#include "CollisionScene.hpp"
#include "CurrentAction.hpp"
#include "Entity.hpp"
#include "HurtBeam.hpp"
#include "HurtBox.hpp"
#include "ShipContact.hpp"

namespace rtype::game::testing {

/** @returns Every ShipContact a HurtZoneSystem publishes for @p scene in one
 * update. */
[[nodiscard]] std::vector<ShipContact> contactsOf(CollisionScene& scene);

/** @returns An enemy at (@p centerX, 0) whose hurt zone holds @p boxes. */
engine::Entity enemyWith(CollisionScene& scene, float centerX,
                         const std::vector<HurtBox>& boxes);

/** @returns An enemy at the origin whose hurt zone holds @p beam and no box
 * that can reach a ship, in the action @p current. */
engine::Entity enemyWithBeam(CollisionScene& scene, const HurtBeam& beam,
                             const CurrentAction& current);

}  // namespace rtype::game::testing
