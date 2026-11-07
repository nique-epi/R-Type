#pragma once

#include <cstdint>
#include "BossStatus.hpp"

namespace rtype::client::testing {

/** @returns The status of the tracked boss fighting with @p health of its
 * TRACKED_FULL_HEALTH points. */
[[nodiscard]] network::BossStatus fightingWith(std::uint16_t health);

}  // namespace rtype::client::testing
