#pragma once

#include <cstddef>
#include <cstdint>
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

/**
 * @brief A missile of the server stands for the predicted shot whose firing
 * input lies less than the interval of the gun divided by this from its own:
 * the server applies the inputs of the ticks they arrive in, so it may fire a
 * few inputs before or after the client. Two shots of a gun are a whole
 * interval apart, so no missile of the server lies that near two of them.
 */
constexpr std::uint32_t SHOT_MATCH_DIVISOR = 2;

}  // namespace rtype::client
