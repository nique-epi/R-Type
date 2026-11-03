#pragma once

#include <cstdint>
#include <span>
#include <vector>
#include "FinalPilot.hpp"
#include "GameEnded.hpp"
#include "PilotStatus.hpp"

namespace rtype::client {

/** @returns A pilot per player of @p ended, in its order, with the nickname
 * and the local flag of the pilot of the same number among @p pilots; none
 * for a player they do not list. Ranks are left at 0. */
[[nodiscard]] std::vector<FinalPilot> finalPilotsOf(
    std::span<const PilotStatus> pilots, const network::GameEnded& ended);

/** @returns The pilots, best score first; equal scores share a rank and keep
 * the order of their numbers, the next rank skipping as many. */
[[nodiscard]] std::vector<FinalPilot> rankedPilots(
    std::vector<FinalPilot> pilots);

/** @returns The sum of the scores, UINT32_MAX at most. */
[[nodiscard]] std::uint32_t teamScoreOf(std::span<const FinalPilot> pilots);

}  // namespace rtype::client
