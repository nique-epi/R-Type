#pragma once

#include <chrono>
#include <cstddef>

/** @brief Less than half the lag of LAGGING_NETWORK: no datagram may cross
 * in that time. */
constexpr std::chrono::milliseconds LESS_THAN_HALF_OF_THE_LAG{50};

/** @brief The byte a test writes over CROSSING_BYTES once they are handed
 * over. */
constexpr std::byte OVERWRITING_BYTE{0xFF};

/** @brief The only byte of the second datagram a test hands over, and how
 * many datagrams that test waits for. */
constexpr std::byte SECOND_DATAGRAM_BYTE{0x0D};
constexpr std::size_t BOTH_DATAGRAMS = 2;
