#pragma once

#include <array>
#include <string>
#include "LagometerConstants.hpp"
#include "LagometerFigures.hpp"
#include "LagometerJournalFigures.hpp"

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

/**
 * @returns The line of the journal for a period: its length in seconds, the
 * smoothed round trip and the jitter in whole milliseconds, the datagrams of
 * the server missed of those expected, the frames and those drawn past the
 * newest state with their rate per second, and the range of the delay of the
 * display. A round trip not known yet reads NO_FIGURE_TEXT.
 */
[[nodiscard]] std::string lagometerJournalLine(
    const LagometerJournalFigures& figures);

}  // namespace rtype::client
