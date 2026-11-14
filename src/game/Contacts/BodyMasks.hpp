#pragma once

#include <optional>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "PlacedMask.hpp"
#include "Position.hpp"

namespace rtype::game {

/**
 * @returns The CollisionMask of @p body laid on the grid of the sprite pixels
 * as if the body stood at @p position (placedMaskOf()), or nothing when it has
 * none.
 *
 * The placed mask points into the mask of the sheet, which outlives the body:
 * it stays valid after the body is destroyed or loses its CollisionMask.
 */
[[nodiscard]] std::optional<engine::PlacedMask> bodyMaskAt(
    engine::ComponentRegistry& components, engine::Entity body,
    const Position& position);

}  // namespace rtype::game
