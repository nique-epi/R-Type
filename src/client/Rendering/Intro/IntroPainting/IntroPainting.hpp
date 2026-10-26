#pragma once

#include "IInterfaceCanvas.hpp"
#include "IntroFrame.hpp"

namespace rtype::client {

/** @brief Paints the scene of the intro for one frame, in the order of the
 * mockup: the Bydos, the missiles of the enemies, the ships, the missiles of
 * the players, the bursts, the rings, the charges, the beams, then the speed
 * lines. */
void paintIntroCast(IInterfaceCanvas& canvas, const IntroFrame& frame);

}  // namespace rtype::client
