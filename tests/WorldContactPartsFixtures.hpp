#pragma once

#include <optional>
#include "EntitySheet.hpp"
#include "HitboxParts.hpp"
#include "HurtZone.hpp"
#include "ObstacleContact.hpp"

namespace rtype::game::testing {

/** @returns The hurt zone the contact parts tests give a sheet: a box that
 * hurts all the time and a box of the strike of attack 3. */
[[nodiscard]] HurtZone someHurtZone();

/** @returns The boxes the contact parts tests give a sheet that lists
 * several: SOME_HITBOX_PARTS. */
[[nodiscard]] HitboxParts someHitboxParts();

/** @returns The Drifter of the shipped sheets with the hurt zone @p zone and
 * the marker @p survives the test asks for. */
[[nodiscard]] EntitySheet drifterWith(const std::optional<HurtZone>& zone,
                                      bool survives);

/** @returns The Drifter of the shipped sheets made a wall that does
 * @p contact to a ship, as a sheet of side obstacle reads. */
[[nodiscard]] EntitySheet wallDoing(ObstacleContact contact);

}  // namespace rtype::game::testing
