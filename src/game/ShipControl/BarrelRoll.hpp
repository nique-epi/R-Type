#pragma once

#include <optional>
#include "RolledRows.hpp"
#include "ShipMotion.hpp"

namespace rtype::game {

/**
 * @returns The rows of a frame @p height rows tall a barrel roll shows
 * @p share of the way through it (0 to 1): round(height x |cos|) of them for
 * a whole turn of ROLL_FULL_TURN times @p share, ROLL_THINNEST_ROWS at least,
 * upside down once the cosine is negative.
 *
 * The client draws a rolling ship with these rows and the server lays its
 * collision mask with them, so both flatten the ship the same way.
 */
[[nodiscard]] RolledRows rolledRowsOf(int height, float share);

/** @returns The row of the upright frame that row @p row of @p rolled shows:
 * the nearest, counted from the bottom while the belly shows. */
[[nodiscard]] int sourceRowOf(RolledRows rolled, int row);

/** @returns How far through its barrel roll @p motion is, from 0 to 1, as
 * ROLL_DURATION runs; nothing while it does not roll. */
[[nodiscard]] std::optional<float> rollShareOf(const ShipMotion& motion);

}  // namespace rtype::game
