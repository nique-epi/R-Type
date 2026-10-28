#pragma once

#include <SFML/Graphics/Color.hpp>
#include <cstddef>
#include <optional>
#include "HangarBay.hpp"
#include "MotionPreference.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "TimeConstants.hpp"

/** @returns The bay of player 3 at BAY_TOP_LEFT, with a pilot in this state,
 * or open. */
[[nodiscard]] rtype::client::HangarBay bayOf(
    std::optional<rtype::client::BayReadiness> readiness,
    bool isLocalPlayer = false);

/** @returns What painting a bay asks of a canvas, its animations run for
 * @p elapsed. */
[[nodiscard]] InterfaceCanvasRecord bayPaintingOf(
    const rtype::client::HangarBay& bay, rtype::engine::Duration elapsed,
    rtype::client::MotionPreference motion);

/** @returns How many beacons of a record are of this color: fills of this
 * color as large as a beacon, unlike the strokes of the box around them. */
[[nodiscard]] std::size_t beaconsOfColor(const InterfaceCanvasRecord& record,
                                         sf::Color color);
