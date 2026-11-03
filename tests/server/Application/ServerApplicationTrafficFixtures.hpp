#pragma once

#include <functional>
#include "Endpoint.hpp"
#include "ServerApplication.hpp"
#include "TrafficReport.hpp"

/**
 * @brief Runs a server on the loopback address, lets @p prepare admit or forget
 * the address of a client, then has that client send a ReadyState.
 * @returns What the server counted once it received it, or after
 * THREAD_TIMEOUT.
 */
[[nodiscard]] rtype::network::TrafficReport trafficAfterAReadyState(
    const std::function<void(rtype::server::ServerApplication&,
                             const rtype::network::Endpoint&)>& prepare);
