#pragma once

#include <cstddef>

namespace rtype::server {

/**
 * @brief How many datagrams a loss rate is measured over: the datagrams a
 * player sent, counting the ones that never arrived. Once that many are
 * accounted for, the rate is published and the count starts again.
 */
constexpr std::size_t LOSS_WINDOW_SIZE = 100;

/**
 * @brief The datagrams the first accepted datagram of a player stands for:
 * itself alone, since no sequence of the player is known before it.
 */
constexpr std::size_t FIRST_DATAGRAM_DISTANCE = 1;

}  // namespace rtype::server
