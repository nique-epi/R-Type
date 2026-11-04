#pragma once

#include <cstddef>
#include <exception>
#include <functional>
#include <vector>
#include "Endpoint.hpp"
#include "ServerApplication.hpp"

/**
 * @brief What a test tick handler throws to make the server stop.
 */
class TickFailure final : public std::exception {};

/** @brief What the ticks of a server were handed in
 * sortReadyStateThenRequest().
 */
struct SortedReadyState {
  bool requestReachedATick{};
  bool readyStateReachedATick{};
};

/**
 * @brief Runs a server on the loopback address, lets @p prepare admit or forget
 * the address of a client before the server starts, then has that client send
 * @p readyState, then a connection request.
 * @returns Whether a tick was handed the request within THREAD_TIMEOUT, and
 * whether any tick was handed @p readyState, among the datagrams of the players
 * or the requests.
 */
[[nodiscard]] SortedReadyState sortReadyStateThenRequest(
    const std::function<void(rtype::server::ServerApplication&,
                             const rtype::network::Endpoint&)>& prepare,
    const std::vector<std::byte>& readyState);

/**
 * @brief Runs a server on the loopback address. One client sends it
 * FLOOD_REQUESTS_AROUND_A_CLIENT connection requests, a second client one
 * request, then the first client FLOOD_REQUESTS_AROUND_A_CLIENT more, all at
 * once.
 * @returns Whether a tick was handed the request of the second client within
 * THREAD_TIMEOUT.
 */
[[nodiscard]] bool requestOfAClientReachesATickThroughAFlood();
