#pragma once

#include <string>
#include <vector>
#include "RouletteReading.hpp"
#include "TableRosterRow.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @returns What is left of the phase of the room of @p reading on the clock
 * of the master: until its bets close, until the wheel stops, until the
 * round ends; none once it passed, or without a room. */
[[nodiscard]] engine::Duration phaseTimeLeft(const RouletteReading& reading);

/**
 * @returns What the status line says of @p reading: THE TABLE IS CLOSED
 * without a room; while the bets are open, why the master refused the newest
 * bets of the round or the seat, then NO CHIPS - SEE THE CASHIER when the
 * player holds no chip and laid none, or PLACE YOUR BETS - CLOSE IN and the
 * whole seconds left, a second begun counted; NO MORE BETS - RESULT IN and
 * the seconds left while the wheel turns; at the result, the number and its
 * color, then, two spaces after, + and the payout of the player when it won,
 * - and its stake when it lost, once the master answered its bets.
 */
[[nodiscard]] std::string statusLineOf(const RouletteReading& reading);

/**
 * @returns A row per player seated in the room of @p reading, the player
 * first, then the others in the order of the room, ROSTER_MAXIMUM_ROWS at
 * most: its color, by its place in the room, its nickname, followed by (YOU)
 * for the player, and its stake, in gold; once the round paid, its payout
 * after + in green, or its stake after - in pale blue.
 */
[[nodiscard]] std::vector<TableRosterRow> rosterRowsOf(
    const RouletteReading& reading);

}  // namespace rtype::client
