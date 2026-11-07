#pragma once

#include "EntitySheets.hpp"

namespace rtype::server::testing {

/** @returns The sheets the game ships and the warlord, a boss; they live as
 * long as the test program. */
[[nodiscard]] const game::EntitySheets& sheetsWithAWarlord();

}  // namespace rtype::server::testing
