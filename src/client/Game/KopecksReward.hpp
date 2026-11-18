#pragma once

#include <cstdint>
#include <span>
#include "FinalPilot.hpp"
#include "GameEnded.hpp"

namespace rtype::client {

/** @returns The kopecks a game that ended with @p outcome earns the pilot of
 * this client among @p pilots: those of its final score, with the bonus of a
 * victory; none when no pilot is this client. */
[[nodiscard]] std::uint32_t kopecksEarnedBy(std::span<const FinalPilot> pilots,
                                            network::WireGameOutcome outcome);

}  // namespace rtype::client
