#pragma once

#include <vector>
#include "CollisionScene.hpp"
#include "Entity.hpp"
#include "HurtBox.hpp"
#include "ShipContact.hpp"

namespace rtype::game::testing {

/** @returns Every ShipContact a HurtZoneSystem publishes for @p scene in one
 * update. */
[[nodiscard]] std::vector<ShipContact> contactsOf(CollisionScene& scene);

/** @returns An enemy at (@p centerX, 0) whose hurt zone holds @p boxes. */
engine::Entity enemyWith(CollisionScene& scene, float centerX,
                         const std::vector<HurtBox>& boxes);

}  // namespace rtype::game::testing
