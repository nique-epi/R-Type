#pragma once

#include <chrono>

namespace rtype::client {

/**
 * @brief How long the thread waits before it reads again a link when no
 * master answered, at first, and at most: each failure doubles the wait, as
 * RFC 8628 (section 3.5) recommends for a connection that times out.
 */
constexpr std::chrono::seconds FIRST_UNREACHABLE_WAIT{5};
constexpr std::chrono::seconds LONGEST_UNREACHABLE_WAIT{60};

/** @brief How many times longer the wait grows after each failure. */
constexpr int UNREACHABLE_WAIT_GROWTH = 2;

}  // namespace rtype::client
