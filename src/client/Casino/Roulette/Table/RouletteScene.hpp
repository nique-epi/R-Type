#pragma once

#include "RouletteBarStates.hpp"
#include "RouletteReading.hpp"
#include "RouletteView.hpp"

namespace rtype::client {

/**
 * @returns What the table shows of @p reading: the croupier inviting the
 * bets while they are open; the wheel at its moment (wheelPictureAt()); the
 * bets of the player; the cursor while the bets are open; the number drawn
 * at the result; the players seated (rosterRowsOf()) under the name of the
 * room; the numbers drawn before, the one drawn at the result first; and the
 * band. The band shows, as chips, those at hand less the stake laid while the
 * bets are open, those the master left once it took the bets while the wheel
 * turns, and those of the wallet at the result; as stake, the stake laid
 * while the bets are open, then the one the master took.
 */
[[nodiscard]] RouletteView rouletteViewOf(const RouletteReading& reading);

/** @returns The bars @p reading opens, only while the bets are open: CLEAR
 * when the player laid some, REBET when the bets of the round before fit in
 * the chips at hand. */
[[nodiscard]] RouletteBarStates barStatesOf(const RouletteReading& reading);

}  // namespace rtype::client
