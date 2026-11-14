#pragma once

#include <optional>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "PlacedMask.hpp"
#include "Position.hpp"

namespace rtype::game {

/**
 * @returns The mask @p body shows, laid on the grid of the sprite pixels as if
 * the body stood at @p position (placedMaskOf()), or nothing when it has none:
 * its CollisionMask, or, while its ShipMotion rolls, the mask of its
 * ShipRollMasks the client draws at that point of the roll (rollShareOf()).
 *
 * The placed mask points into the mask of the sheet, which outlives the body:
 * it stays valid after the body is destroyed or loses its CollisionMask.
 */
[[nodiscard]] std::optional<engine::PlacedMask> bodyMaskAt(
    engine::ComponentRegistry& components, engine::Entity body,
    const Position& position);

/**
 * @brief Empties the CollisionMask of @p body, when it has one: from then on
 * it is touched anywhere in its CollisionBox, as a body of a sheet without a
 * mask is.
 *
 * Its ShipRollMasks, when it has one, is emptied too.
 *
 * A script that resizes the box of a body calls it, since the mask was cut to
 * the box of the sheet: the size a script asks for is the size the body is
 * touched on. The component stays, so this is safe inside a query of the
 * registry, which forbids removing a component.
 */
void forgetMaskOf(engine::ComponentRegistry& components, engine::Entity body);

}  // namespace rtype::game
