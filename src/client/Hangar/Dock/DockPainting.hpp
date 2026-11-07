#pragma once

#include <cstddef>
#include <optional>
#include "DockScene.hpp"
#include "IInterfaceCanvas.hpp"
#include "MotionPreference.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @returns The beacon of the bay lit at @p elapsed, one after the other
 * every BEACON_STEP; none under reduced motion. */
[[nodiscard]] std::optional<int> litBeaconOf(engine::Duration elapsed,
                                             MotionPreference motion);

/** @brief Paints the wall of the bay: panels, the light from its ceiling,
 * the rings of its floor and its beacons, @p litBeacon lit. */
void paintDockWall(IInterfaceCanvas& canvas, std::optional<int> litBeacon);

/** @brief Paints the reference card of @p item at BLUEPRINT_AREA: its
 * drawing in cyan on blueprint paper, and its size in pixels. */
void paintBlueprint(IInterfaceCanvas& canvas, std::size_t item);

/** @brief Paints the four arms and the ship of @p scene, what it wears, the
 * item an arm brings and the sparks of an item placed or taken off. */
void paintDock(IInterfaceCanvas& canvas, const DockScene& scene);

}  // namespace rtype::client
