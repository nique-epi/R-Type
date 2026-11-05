#pragma once

#include "HurtZone.hpp"

namespace rtype::game::testing {

/** @returns The hurt zone the contact parts tests give a sheet: a box that
 * hurts all the time and a box of the strike of attack 3. */
[[nodiscard]] HurtZone someHurtZone();

}  // namespace rtype::game::testing
