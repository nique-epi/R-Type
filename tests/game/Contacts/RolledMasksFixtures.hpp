#pragma once

#include <memory>
#include "CollisionScene.hpp"
#include "Entity.hpp"
#include "PixelMask.hpp"

namespace rtype::game::testing {

/** @returns A mask @p side pixels a side with one pixel drawn per row, on
 * its diagonal: row r has its column r alone drawn, so a row of a rolled mask
 * tells which row of it it copies. */
[[nodiscard]] engine::PixelMask diagonalMask(int side);

/** @returns A ship of @p scene at the origin whose CollisionMask is
 * @p upright and whose ShipRollMasks are the masks of its roll. */
engine::Entity shipRollingOn(
    CollisionScene& scene,
    const std::shared_ptr<const engine::PixelMask>& upright);

}  // namespace rtype::game::testing
