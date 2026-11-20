#pragma once

#include <optional>
#include "BiomeGround.hpp"
#include "GroundPlace.hpp"

namespace rtype::game {

/**
 * @returns The row of the top of the floor of @p ground under @p place, in
 * logical units from the top of the playfield; nothing where the column has
 * no floor, or lies left of where the first map of the biome began.
 *
 * The maps are placed as the client draws the near plane of a background
 * that arrives through its entrance: the first begins GROUND_ENTRANCE_PIXEL
 * pixels from the left edge as the biome begins, moves left with the scroll,
 * and the maps come back to the first after the last.
 */
[[nodiscard]] std::optional<float> floorRowAt(const BiomeGround& ground,
                                              GroundPlace place);

/** @returns The row just under the ceiling of @p ground under @p place, as
 * floorRowAt() places it; nothing where the column has no ceiling. */
[[nodiscard]] std::optional<float> ceilingRowAt(const BiomeGround& ground,
                                                GroundPlace place);

}  // namespace rtype::game
