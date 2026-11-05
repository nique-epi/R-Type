#pragma once

#include <cstddef>
#include <cstdint>
#include "ServerConstants.hpp"

namespace rtype::server::testing {

/** @brief A fingerprint other than the one of the shipped waves file. */
constexpr std::uint64_t ANOTHER_WAVES_FINGERPRINT =
    OFFICIAL_WAVES_FINGERPRINT + 1;

/** @brief A seed of 0, which is a seed all the same. */
constexpr std::uint32_t ZERO_SEED = 0;

/** @brief An input delay one tick on each side of the default one. */
constexpr std::size_t ONE_TICK_SHORTER_DELAY = DEFAULT_INPUT_DELAY_TICKS - 1;
constexpr std::size_t ONE_TICK_LONGER_DELAY = DEFAULT_INPUT_DELAY_TICKS + 1;

}  // namespace rtype::server::testing
