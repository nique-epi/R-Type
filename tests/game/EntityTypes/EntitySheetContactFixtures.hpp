#pragma once

#include "EntitySheet.hpp"
#include "HurtZone.hpp"

namespace rtype::game::testing {

/** @returns The hurt zone of @p sheet, an empty one, and a failure, when it
 * has none. */
[[nodiscard]] const HurtZone& zoneOf(const EntitySheet& sheet);

}  // namespace rtype::game::testing
