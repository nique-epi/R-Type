#pragma once

#include <cstdint>
#include "RouletteBarStates.hpp"
#include "RouletteReading.hpp"
#include "RouletteView.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @returns What the table shows of @p reading: the croupier inviting the
 * bets while they are open; the wheel at its moment (wheelPictureAt()); the
 * bets of the player; the cursor while the bets are open; the number drawn
 * at the result; the pilots seated (rosterRowsOf()); the numbers drawn
 * before; and the band. The band shows, as chips, those of the settings less
 * the stake laid while the bets are open, those held when they closed while
 * the wheel turns, and those of the settings at the result; as stake, the
 * stake laid while the bets are open, then the one the table took.
 */
[[nodiscard]] RouletteView rouletteViewOf(const RouletteReading& reading);

/** @returns The bars @p reading opens, all of them only while the bets are
 * open: SPIN when the player holds chips or laid some, CLEAR when it laid
 * some, REBET when the bets of the round before fit in the chips it holds;
 * and whether SPIN shows the vote of the player. */
[[nodiscard]] RouletteBarStates barStatesOf(const RouletteReading& reading);

/** @returns What is left of a phase that had @p ticksLeft ticks of the
 * server left @p since ago; none once it passed. */
[[nodiscard]] engine::Duration phaseTimeLeft(std::uint16_t ticksLeft,
                                             engine::Duration since);

}  // namespace rtype::client
