#pragma once

#include <cstdint>
#include <optional>
#include "FlashPreference.hpp"
#include "IInterfaceCanvas.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief How strongly the edges of the playfield are tinted, on the edge
 * itself: the alpha of HAZARD_RED there, 0 for clear edges. */
struct EdgePulseLook {
  std::uint8_t alpha{};

  bool operator==(const EdgePulseLook&) const = default;
};

/**
 * @returns The tint of the edges at that time since the Arriving status, and
 * since the roar of the boss when it roared: the pulse of the WARNING screen
 * (see EDGE_PULSE_PERIOD), or the flash of the roar when it is stronger.
 * Clear with FlashPreference::Reduced.
 */
[[nodiscard]] EdgePulseLook edgePulseAt(
    engine::Duration sinceArrival, std::optional<engine::Duration> sinceRoar,
    FlashPreference flashes);

/** @brief Tints the four edges of ALERT_PLAYFIELD_AREA, never the status
 * band; nothing for clear edges. */
void paintEdgePulse(IInterfaceCanvas& canvas, EdgePulseLook look);

}  // namespace rtype::client
