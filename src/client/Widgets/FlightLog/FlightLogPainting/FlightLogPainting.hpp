#pragma once

#include "FlightLogLook.hpp"
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/** @brief Paints the flight log as it looks: the book, its pages while it
 * lies open, the label of the taunt, the dust of the landing, the choices. */
void paintFlightLog(IInterfaceCanvas& canvas, const FlightLogLook& look);

}  // namespace rtype::client
