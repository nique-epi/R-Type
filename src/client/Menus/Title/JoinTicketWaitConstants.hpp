#pragma once

#include <chrono>
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief How long an attempt of a signed-in player waits for the master to
 * give it a game ticket before it joins as a guest: a master that is slow or
 * down never keeps a player out of a game for longer, and a game joined as a
 * guest only pays nothing.
 */
constexpr engine::Duration TICKET_WAIT = std::chrono::seconds{2};

}  // namespace rtype::client
