#pragma once

#include <string>
#include "LagometerJournalFigures.hpp"

namespace rtype::client {

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
