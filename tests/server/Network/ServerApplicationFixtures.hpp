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

/** @returns A ReadyState carrying SESSION_TOKEN, as an admitted player sends.
 */
[[nodiscard]] std::vector<std::byte> playerReady();

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
