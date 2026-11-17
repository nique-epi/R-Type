#pragma once

#include <httplib.h>
#include <cstdint>
#include "RouteCall.hpp"

/** @returns The answer of the master listening on @p port to @p call, sent as
 * JSON, with its key as Bearer when it has one, as forwarded from a client
 * over HTTPS when it says so, and with the site headers it holds. */
[[nodiscard]] httplib::Result sendRouteCall(std::uint16_t port,
                                            const RouteCall& call);
