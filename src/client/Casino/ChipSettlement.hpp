#pragma once

#include <cstdint>
#include "ChipMoves.hpp"
#include "ICasinoChannel.hpp"
#include "SettingsService.hpp"

namespace rtype::client {

/** @returns The chips of a player who held @p chips once @p moves are
 * settled: the stakes taken off, never below none, then the payouts added,
 * MAXIMUM_CHIPS at most. Computed in 64 bits, so no count the server sends
 * wraps around. */
[[nodiscard]] std::uint32_t chipsAfter(std::uint32_t chips, ChipMoves moves);

/**
 * @brief Settles in the chips of @p settings what the rounds of @p casino
 * moved since the last call, and saves the settings when they changed.
 * Called once per frame, whichever screen shows, so a round is settled even
 * when the table is not on screen; it does nothing when no round moved a
 * chip.
 */
void settleChipMoves(ICasinoChannel& casino, SettingsService& settings);

}  // namespace rtype::client
