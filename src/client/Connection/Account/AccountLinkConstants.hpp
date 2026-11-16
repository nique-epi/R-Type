#pragma once

#include <chrono>
#include <cstdint>

namespace rtype::client {

/** @brief How long the thread of a MasterAccountLink sleeps between two
 * looks at its stop when nothing is asked. */
constexpr std::chrono::seconds ACCOUNT_ASK_WAIT{1};

/** @brief The number of no ask: the link numbers the asks of each kind from
 * 1, so no reading ever answers it. */
constexpr std::uint64_t NO_ACCOUNT_ASK = 0;

}  // namespace rtype::client
