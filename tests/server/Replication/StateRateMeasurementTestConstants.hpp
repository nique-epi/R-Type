#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace rtype::server::testing {

/**
 * @brief How long each measured rate runs, in seconds of game.
 */
constexpr std::size_t MEASURED_SECONDS = 10;

/**
 * @brief The players and the networked entities of the measured game: a full
 * room and a screen with a few dozen entities.
 */
constexpr std::size_t MEASURED_PLAYERS = 4;
constexpr std::uint32_t MEASURED_ENTITIES = 30;

/**
 * @brief The rates the measurement compares.
 */
constexpr std::array<std::size_t, 5> MEASURED_STATE_RATES{2, 10, 20, 30, 60};

/**
 * @brief One datagram in this many is lost on its way to the measured player,
 * and the seed that picks which ones.
 */
constexpr std::uint32_t ONE_LOST_IN = 10;
constexpr std::uint32_t LOSS_SEED = 7;

/**
 * @brief The places in MEASURED_STATE_RATES of the rate the others are compared
 * with (20) and of the rate three times as high (60), and by how much the bytes
 * per second of the second may differ from three times those of the first.
 */
constexpr std::size_t REFERENCE_RATE_INDEX = 2;
constexpr std::size_t TRIPLE_RATE_INDEX = 4;
constexpr double TRIPLE = 3.0;
constexpr double RATIO_TOLERANCE = 0.02;

/**
 * @brief Milliseconds in a second, to print a gap in the unit a player feels.
 */
constexpr double MILLISECONDS_PER_SECOND = 1000.0;

}  // namespace rtype::server::testing
