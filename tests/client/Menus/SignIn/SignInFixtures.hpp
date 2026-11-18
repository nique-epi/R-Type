#pragma once

#include <chrono>
#include <cstdint>
#include "DeviceLinkPhase.hpp"
#include "DeviceLinkReading.hpp"

namespace rtype::client::testing {

/** @returns The link of the ask @p ask showing SHOWN_CODE on SHOWN_PAGE,
 * waiting, until @p expiresAt. */
[[nodiscard]] DeviceLinkReading waitingReading(
    std::uint64_t ask, std::chrono::sys_seconds expiresAt);

/** @returns The link of the ask @p ask at @p phase, holding SIGN_IN_TOKEN
 * once SignedIn. */
[[nodiscard]] DeviceLinkReading readingAt(std::uint64_t ask,
                                          DeviceLinkPhase phase);

}  // namespace rtype::client::testing
