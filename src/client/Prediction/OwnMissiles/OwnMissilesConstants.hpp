#pragma once

#include <cstddef>
#include "ProtocolConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The ticks a shot of the player may wait for its EntitySpawned once
 * the server applied the input that fired it: two sends again of a guaranteed
 * message, each one tick after GUARANTEED_RESEND_DELAY (7 ticks), so an
 * EntitySpawned lost twice still arrives first. A shot still unanswered
 * after them was refused by the server, and its missile is taken away.
 */
constexpr std::size_t REFUSED_SHOT_TICKS =
    2 * (engine::wholeTicksIn(network::GUARANTEED_RESEND_DELAY) + 1);

}  // namespace rtype::client
