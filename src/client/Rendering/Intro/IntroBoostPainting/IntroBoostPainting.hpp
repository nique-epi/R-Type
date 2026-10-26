#pragma once

#include "IInterfaceCanvas.hpp"
#include "IntroFrame.hpp"

namespace rtype::client {

/** @brief Paints the charge at the nose of every ship, from the charge to
 * just past the release: grains of the light shade of the pilot drawn in, and
 * a ball that grows. */
void paintCharges(IInterfaceCanvas& canvas, const IntroFrame& frame);

/** @brief Paints the beam of every ship for INTRO_BEAM_TIME from the release:
 * bands from the dark shade of the pilot to white, waving, reaching past the
 * right edge. */
void paintBeams(IInterfaceCanvas& canvas, const IntroFrame& frame);

/** @brief Paints the speed lines while the stars go faster than
 * INTRO_SPEED_LINE_FLOOR times their speed, more the faster they go; none
 * with REDUCE FLASHES. */
void paintSpeedLines(IInterfaceCanvas& canvas, const IntroFrame& frame);

}  // namespace rtype::client
