#pragma once

#include <chrono>
#include "ProtocolConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief How much later than its first copy a copy of a GameEvent may arrive,
 * beyond the lifetime of a guaranteed message: two copies take different
 * delays, far less than this apart on the worst network of the scenarios
 * (50 ms of jitter). */
constexpr engine::Duration GAME_EVENT_DELAY_SPREAD = std::chrono::seconds(1);

/** @brief How long the number of a GameEvent is remembered: the server sends a
 * guaranteed message again for up to GUARANTEED_MESSAGE_LIFETIME, and a copy
 * may travel GAME_EVENT_DELAY_SPREAD longer than the first. */
constexpr engine::Duration GAME_EVENT_MEMORY =
    network::GUARANTEED_MESSAGE_LIFETIME + GAME_EVENT_DELAY_SPREAD;

}  // namespace rtype::client
