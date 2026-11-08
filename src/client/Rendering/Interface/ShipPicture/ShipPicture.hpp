#pragma once

#include "IInterfaceCanvas.hpp"
#include "PilotShipPicture.hpp"
#include "ShipPlacement.hpp"

namespace rtype::client {

/** @brief Draws the frame of @p picture, in the row of its color of the
 * entity sheet, recolored to its palette, at @p ship. */
void paintPilotShip(IInterfaceCanvas& canvas, const PilotShipPicture& picture,
                    ShipPlacement ship);

}  // namespace rtype::client
