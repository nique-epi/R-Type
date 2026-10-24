#pragma once

#include <cstddef>
#include "ProtocolConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief UNLISTED_ENTITY_TIMEOUT in whole server ticks, rounded down: 54 ticks
 * at 60 per second. The client counts the ticks of the states it receives, not
 * the time that passes, so while no state arrives (a stall of the server, a
 * burst of losses) no entity ages, and the first newer state that still leaves
 * an entity out removes it.
 */
constexpr std::size_t UNLISTED_ENTITY_TICKS =
    engine::wholeTicksIn(network::UNLISTED_ENTITY_TIMEOUT);

static_assert(UNLISTED_ENTITY_TICKS >= 1,
              "UNLISTED_ENTITY_TIMEOUT must last at least one simulation tick");

}  // namespace rtype::client
