#pragma once

#include <cstdint>

/** @returns Whether a UDP socket can be bound on @p port, on every IPv4
 * interface, right now. */
[[nodiscard]] bool udpPortIsFree(std::uint16_t port);

/** @returns A UDP port nothing was bound to a moment ago, picked by the
 * system. */
[[nodiscard]] std::uint16_t freeUdpPort();
