#pragma once

#include <cstddef>
#include <span>
#include "PilotHull.hpp"
#include "ShipSheetFrame.hpp"
#include "SpriteFrame.hpp"

namespace rtype::client {

/**
 * @returns Where the ship of @p pilot shows @p frame, a frame of the ship of
 * the first pilot color (one of PLAYER_SHIP_FRAMES): on the entity sheet, in
 * the row of its color, for the standard hull; on the hull sheet, in the row
 * of its hull and its color, for a hull of the catalog. A number that names
 * no hull, or a color outside the pilot colors, gives the standard hull.
 */
[[nodiscard]] ShipSheetFrame pilotShipFrameOf(PilotHull pilot,
                                              game::SpriteFrame frame);

/**
 * @returns The frames the hull of @p pilot plays, in the row of its color of
 * the hull sheet; none for the standard hull, whose frames its entity sheet
 * gives. The frames are a constant: the view never dangles.
 */
[[nodiscard]] std::span<const game::SpriteFrame> hullFramesOf(PilotHull pilot);

/**
 * @returns The picture that stands for the hull @p item, a position in
 * SHIP_LOOK_ITEM_NAMES, in a catalogue (the locker, the card of the hangar,
 * the glass case of the shop): its ship, still, in HULL_ITEM_COLOR.
 */
[[nodiscard]] ShipSheetFrame hullItemFrameOf(std::size_t item);

}  // namespace rtype::client
