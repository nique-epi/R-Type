#pragma once

#include <SFML/System/Vector2.hpp>
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

/** @returns What a feed that followed @p pilot paints from @p topLeft down. */
[[nodiscard]] InterfaceCanvasRecord feedPaintingFrom(
    const rtype::client::PilotStatus& pilot, sf::Vector2i topLeft);
