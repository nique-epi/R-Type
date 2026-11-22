#pragma once

#include <cstdint>
#include <span>
#include "FinalPilot.hpp"

namespace rtype::client {

/** @returns The kopecks a game earns the pilot of this client among
 * @p pilots: those of its final score and its bounty, as the master credits
 * them; none when no pilot is this client. */
[[nodiscard]] std::uint32_t kopecksEarnedBy(std::span<const FinalPilot> pilots);

}  // namespace rtype::client
