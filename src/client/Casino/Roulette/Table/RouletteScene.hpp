#pragma once

#include <optional>
#include "BigWin.hpp"
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

/** @returns The biggest win of the round of @p reading at its result: the
 * player seated whose payout passes its stake by BIG_WIN_GAIN chips or more,
 * the one who gained the most when several did; nothing before the result,
 * or when nobody won that much. */
[[nodiscard]] std::optional<BigWin> bigWinOf(const RouletteReading& reading);

/** @returns The bars @p reading opens, only while the bets are open: CLEAR
 * when the player laid some, REBET when the bets of the round before fit in
 * the chips at hand. */
[[nodiscard]] RouletteBarStates barStatesOf(const RouletteReading& reading);

}  // namespace rtype::client
