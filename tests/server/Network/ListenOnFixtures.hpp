#pragma once

#include <cstdint>
#include "Endpoint.hpp"
#include "ServerApplication.hpp"

/** @returns A tick handler that ignores the datagrams it is handed. */
rtype::server::ServerApplication::TickHandler ignoringTicks();

/** @returns The loopback address on @p port. */
rtype::network::Endpoint loopbackEndpoint(std::uint16_t port);

/** @returns The unusable address, on any port. */
rtype::network::Endpoint unusableEndpoint();
