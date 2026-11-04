#pragma once

#include <cstddef>
#include "TabStrip.hpp"

/** @returns The four tabs of the options, @p open open, at STRIP_TOP_LEFT. */
[[nodiscard]] rtype::client::TabStrip optionsTabs(std::size_t open);

/** @returns The two tabs of the leaderboard, the first open, NARROW_WIDTH
 * wide. */
[[nodiscard]] rtype::client::TabStrip narrowStrip();
