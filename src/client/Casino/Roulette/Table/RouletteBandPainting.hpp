#pragma once

#include "IInterfaceCanvas.hpp"
#include "RouletteBand.hpp"

namespace rtype::client {

/** @brief Paints @p band across the foot of the screen, but its bars: its
 * rules of gold, the chips in gold and the stake in white, each under its
 * caption, the status line, then the chips of the selector, the chosen one
 * raised. */
void paintRouletteBand(IInterfaceCanvas& canvas, const RouletteBand& band);

}  // namespace rtype::client
