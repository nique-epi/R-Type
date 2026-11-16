#pragma once

#include <chrono>

namespace rtype::client {

/** @brief How long the thread of a MasterCasinoLink sleeps between two looks
 * at its stop when nothing is asked. */
constexpr std::chrono::seconds CASINO_ASK_WAIT{1};

}  // namespace rtype::client
