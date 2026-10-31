#pragma once

#include <chrono>
#include <cstddef>

/** @brief Less than half the lag of LAGGING_NETWORK: no datagram may cross
 * in that time. */
constexpr std::chrono::milliseconds LESS_THAN_HALF_OF_THE_LAG{50};

/** @brief The byte a test writes over CROSSING_BYTES once they are handed
 * over. */
constexpr std::byte OVERWRITING_BYTE{0xFF};
