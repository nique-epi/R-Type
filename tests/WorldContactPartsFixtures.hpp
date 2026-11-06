#pragma once

#include <optional>
#include "EntitySheet.hpp"
#include "HurtZone.hpp"

namespace rtype::game::testing {

/** @returns The hurt zone the contact parts tests give a sheet: a box that
 * hurts all the time and a box of the strike of attack 3. */
[[nodiscard]] HurtZone someHurtZone();

/** @returns The Drifter of the shipped sheets with the hurt zone @p zone and
 * the marker @p survives the test asks for. */
[[nodiscard]] EntitySheet drifterWith(const std::optional<HurtZone>& zone,
                                      bool survives);

}  // namespace rtype::game::testing
