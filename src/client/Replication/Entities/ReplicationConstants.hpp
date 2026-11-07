#pragma once

#include <cstddef>
#include "ProtocolConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief UNLISTED_ENTITY_TIMEOUT in whole server ticks, rounded down: 54 ticks
 * at 60 per second. The client counts the ticks of the states it receives, not
 * the time that passes, so while no state arrives (a stall of the server, a
 * burst of losses) no entity ages. An entity is removed once the parts received
 * for a closed tick, this many ticks or more after its newest listing, leave it
 * out.
 */
constexpr std::size_t UNLISTED_ENTITY_TICKS =
    engine::wholeTicksIn(network::UNLISTED_ENTITY_TIMEOUT);

static_assert(UNLISTED_ENTITY_TICKS >= 1,
              "UNLISTED_ENTITY_TIMEOUT must last at least one simulation tick");

/**
 * @brief The most missiles of its player a client keeps claimed: a ship fires
 * at most 3 per second (20 ticks apart) and a missile crosses the playfield in
 * 2 s, so 6 fly at once; 32 leaves room for missiles whose EntityDestroyed was
 * lost.
 */
constexpr std::size_t CLAIMED_MISSILE_COUNT = 32;

}  // namespace rtype::client
