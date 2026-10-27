#pragma once

#include "BookPose.hpp"
#include "FlightLogLook.hpp"
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/** @brief Paints the label of the taunt on the cover of the book in @p pose,
 * its lines typed letter by letter; it falls off once the cover starts to
 * turn. */
void paintTauntLabel(IInterfaceCanvas& canvas, const FlightLogLook& look,
                     BookPose pose);

}  // namespace rtype::client
