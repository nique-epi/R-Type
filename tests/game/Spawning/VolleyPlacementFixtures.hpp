#pragma once

#include <cstdint>
#include "VolleySheet.hpp"

namespace rtype::game::testing {

/** @returns The burst of the Tyrant, of @p count bodies, as its sheet fires
 * it: from its mouth, spread over 48 degrees, 8 units out. */
[[nodiscard]] VolleySheet burstOf(std::uint8_t count);

/** @returns A column of three bodies 100 units left of the center. */
[[nodiscard]] VolleySheet column();

/** @returns A wall of nine bodies whose second and third are the gap. */
[[nodiscard]] VolleySheet wallWithAGap();

}  // namespace rtype::game::testing
