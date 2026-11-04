#pragma once

#include <cstdint>

/** @returns A TCP port nothing listened on a moment ago, on every interface. */
[[nodiscard]] std::uint16_t freeHttpPort();

/**
 * @returns Whether an HTTP server of the master can listen on @p port of every
 * interface now.
 */
[[nodiscard]] bool httpPortIsFree(std::uint16_t port);
