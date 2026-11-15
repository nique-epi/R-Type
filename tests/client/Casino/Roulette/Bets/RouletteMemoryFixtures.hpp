#pragma once

#include <cstdint>
#include "RouletteRound.hpp"

namespace rtype::client::testing {

/** @returns Round @p number of the table in @p phase, drawing
 * @p winningNumber, nobody seated. */
[[nodiscard]] network::RouletteRound roundAt(std::uint16_t number,
                                             network::RoulettePhase phase,
                                             std::uint8_t winningNumber);

}  // namespace rtype::client::testing
