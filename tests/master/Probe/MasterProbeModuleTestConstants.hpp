#pragma once

#include <chrono>
#include <cstddef>

/**
 * @brief How long a test waits for probes: far longer than a datagram takes
 * on the loopback, so a probe that does not come within it was never sent.
 */
constexpr std::chrono::milliseconds PROBE_DEADLINE{1'000};

/** @brief The probes a test waits for. */
constexpr std::size_t ONE_PROBE = 1;
constexpr std::size_t TWO_PROBES = 2;
