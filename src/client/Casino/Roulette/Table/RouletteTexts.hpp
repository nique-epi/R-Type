#pragma once

#include <optional>
#include <string>
#include <vector>
#include "RouletteReading.hpp"
#include "RouletteRound.hpp"
#include "TableRosterRow.hpp"

namespace rtype::client {

/** @returns The seat of the player at the table @p reading read; none when it
 * is not seated, or without a table. */
[[nodiscard]] std::optional<network::RouletteSeat> ownSeatOf(
    const RouletteReading& reading);

/**
 * @returns What the status line says of @p reading: THE TABLE IS CLOSED
 * without a table; while the bets are open, NO CHIPS - SEE THE CASHIER when
 * the player holds no chip and laid none, or PLACE YOUR BETS - CLOSE IN and
 * the whole seconds left, a second begun counted; NO MORE BETS while the
 * wheel turns; at the result, the number and its color, then, two spaces
 * after, + and the payout of the player when it won, - and its stake when it
 * lost.
 */
[[nodiscard]] std::string statusLineOf(const RouletteReading& reading);

/**
 * @returns A row per pilot seated at the table of @p reading, in the order
 * of the table: its color, its name from the waiting room, followed by
 * (YOU) for the player, SPIN when the player asked to spin, and its stake,
 * in gold; at the result, its payout after + in green, or its stake after -.
 * A seat whose number has no player color is left out.
 */
[[nodiscard]] std::vector<TableRosterRow> rosterRowsOf(
    const RouletteReading& reading);

}  // namespace rtype::client
