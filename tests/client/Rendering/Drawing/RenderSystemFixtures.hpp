#pragma once

#include <string>
#include <vector>
#include "ShipFlight.hpp"

/** @returns The images of the seven layers, one per layer, from the
 * background to the interface: the order RenderSystem must draw them in. */
std::vector<std::string> allLayersInOrder();

/** @returns The ShipFlight of a ship rolling, its roll drawn HALF_A_ROLL of
 * the way. */
rtype::client::ShipFlight halfwayRollingFlight();

/** @returns The ShipFlight of a ship dashing, its whole trail known, at
 * (SHIP_X, SHIP_Y). */
rtype::client::ShipFlight dashingFlight();
