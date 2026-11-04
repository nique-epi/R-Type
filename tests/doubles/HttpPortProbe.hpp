#pragma once

#include <httplib.h>
#include <cstdint>
#include <memory>
#include <string_view>
#include "MasterHttpServer.hpp"

/** @returns An HTTP server of the master that reports nothing when it fails. */
[[nodiscard]] rtype::master::MasterHttpServer quietHttpServer();

/** @returns A TCP port nothing listened on a moment ago, on every interface. */
[[nodiscard]] std::uint16_t freeHttpPort();

/**
 * @returns Whether an HTTP server of the master can listen on @p port of every
 * interface now.
 */
[[nodiscard]] bool httpPortIsFree(std::uint16_t port);

/** @returns The answer of the server on the loopback and @p port to GET @p
 * path. */
[[nodiscard]] httplib::Result httpGet(std::uint16_t port,
                                      std::string_view path);

/** @returns The answer of the server on the loopback and @p port to @p request.
 */
[[nodiscard]] httplib::Result httpSend(std::uint16_t port,
                                       const httplib::Request& request);

/**
 * @returns A client of the server on the loopback and @p port that asked GET
 * @p path and keeps its connection open while it lives, or nullptr when the
 * server did not answer.
 */
[[nodiscard]] std::unique_ptr<httplib::Client> keptAliveClient(
    std::uint16_t port, std::string_view path);
