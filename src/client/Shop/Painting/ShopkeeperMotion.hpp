#pragma once

#include "ShopkeeperPose.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @returns How the shopkeeper stands once the shop was shown for
 * @p elapsed: where his breath (SHOPKEEPER_DROPS) and his eye
 * (SHOPKEEPER_EYE_CUES) are in their cycles, counted in whole steps of
 * SHOPKEEPER_STEP. */
[[nodiscard]] ShopkeeperPose shopkeeperPoseAt(engine::Duration elapsed);

}  // namespace rtype::client
