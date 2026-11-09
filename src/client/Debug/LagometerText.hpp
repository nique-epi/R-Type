#pragma once

#include <array>
#include <string>
#include "LagometerConstants.hpp"
#include "LagometerFigures.hpp"

namespace rtype::client {

/**
 * @returns The four lines of the panel: the smoothed round trip and the
 * jitter in whole milliseconds, the share of datagrams missed and the frames
 * per second, the kilobytes received and sent per second, then how far behind
 * the server the other entities are drawn and the frames per second drawn
 * past the newest state. A figure not known yet reads NO_FIGURE_TEXT.
 */
[[nodiscard]] std::array<std::string, LAGOMETER_TEXT_LINE_COUNT>
lagometerTextLines(const LagometerFigures& figures);

}  // namespace rtype::client
