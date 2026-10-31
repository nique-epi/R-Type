#pragma once

#include <memory>
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"
#include "NetworkConditions.hpp"
#include "NetworkContext.hpp"

namespace rtype::network {

/**
 * @brief Opens the UDP socket of a program on @p localEndpoint, behind a
 * simulated network unless @p conditions is PERFECT_NETWORK.
 *
 * For PERFECT_NETWORK it returns the UdpSocket itself, so a program launched
 * without simulator option pays nothing for the simulator. Otherwise it
 * returns a SimulatedNetworkSocket over it, and logs a warning that names the
 * conditions, so a journal shows the network was simulated.
 *
 * @throws InvalidAddressException or SocketOpenException, as UdpSocket does.
 */
[[nodiscard]] std::unique_ptr<IDatagramSocket> openDatagramSocket(
    NetworkContext& context, const Endpoint& localEndpoint,
    const NetworkConditions& conditions);

}  // namespace rtype::network
