#pragma once

#include <cstdint>
#include "Endpoint.hpp"

/** @returns Every IPv4 interface on @p port, where udpPortIsFree() and
 * freeUdpPort() bind. */
[[nodiscard]] rtype::network::Endpoint anyInterfaceOn(std::uint16_t port);

/** @returns Whether a UDP socket can be bound on @p port, on every IPv4
 * interface, right now. */
[[nodiscard]] bool udpPortIsFree(std::uint16_t port);

/** @returns A UDP port nothing was bound to a moment ago, picked by the
 * system. */
[[nodiscard]] std::uint16_t freeUdpPort();
