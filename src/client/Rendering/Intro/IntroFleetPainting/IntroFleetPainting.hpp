#pragma once

#include "IInterfaceCanvas.hpp"
#include "IntroFrame.hpp"

namespace rtype::client {

/** @brief Paints every Bydo alive at the moment of the frame, those the
 * missiles are about to hit tinted lilac, all left behind by the boost. */
void paintMobs(IInterfaceCanvas& canvas, const IntroFrame& frame);

/** @brief Paints every ship on screen: its ghosts as it enters and at the
 * boost, its flame, then its sprite, in the color of its pilot. */
void paintShips(IInterfaceCanvas& canvas, const IntroFrame& frame);

}  // namespace rtype::client
