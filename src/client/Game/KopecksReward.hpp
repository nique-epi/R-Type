#pragma once

#include <cstdint>
#include <span>
#include "FinalPilot.hpp"
#include "GameEnded.hpp"
#include "SettingsService.hpp"

namespace rtype::client {

/** @returns The kopecks a game that ended with @p outcome earns the pilot of
 * this client among @p pilots: those of its final score, with the bonus of a
 * victory; none when no pilot is this client. */
[[nodiscard]] std::uint32_t kopecksEarnedBy(std::span<const FinalPilot> pilots,
                                            network::WireGameOutcome outcome);

/** @brief Adds @p gain to the balance of the settings, MAXIMUM_KOPECKS at
 * most, and saves them; does nothing for no gain. */
void creditKopecks(SettingsService& settings, std::uint32_t gain);

}  // namespace rtype::client
