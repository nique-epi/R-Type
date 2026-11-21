#pragma once

#include "ShipGun.hpp"

namespace rtype::game::testing {

/** @returns The gun of the shipped ship, as the shipped sheets describe it. */
[[nodiscard]] ShipGun shippedGun();

}  // namespace rtype::game::testing
