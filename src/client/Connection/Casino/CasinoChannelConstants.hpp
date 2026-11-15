#pragma once

#include <chrono>

namespace rtype::client {

/**
 * @brief Time between two copies of a CasinoPlace the server has not shown
 * yet, and between two RouletteBets while the bets are open. Provisional, see
 * the protocol page.
 */
constexpr std::chrono::milliseconds CASINO_MESSAGE_INTERVAL{500};

}  // namespace rtype::client
