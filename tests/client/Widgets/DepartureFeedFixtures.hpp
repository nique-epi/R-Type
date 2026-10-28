#pragma once

#include <cstdint>
#include <string_view>
#include "PilotStatus.hpp"
#include "RecordingInterfaceCanvas.hpp"

/** @returns A pilot of this number and nickname, in this state. */
[[nodiscard]] rtype::client::PilotStatus pilotIn(
    std::uint8_t number, std::string_view nickname,
    rtype::client::PilotState state);

/** @returns What a feed that followed @p pilot paints, in the shades of the
 * sheets. */
[[nodiscard]] InterfaceCanvasRecord feedPaintingOf(
    const rtype::client::PilotStatus& pilot);
