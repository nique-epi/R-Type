#pragma once

#include <vector>
#include "ConnectionRefused.hpp"
#include "ConnectionStatus.hpp"
#include "Endpoint.hpp"
#include "RecordingServerConnection.hpp"
#include "ServerChoice.hpp"

/** @returns The requests to start an attempt the connection received, in
 * order, without the cancels. */
[[nodiscard]] std::vector<ServerConnectionCall> connectAttemptsOf(
    const RecordingServerConnection& connection);

/** @returns The choice of server the launch options give the tests:
 * LAUNCH_ADDRESS, LAUNCH_PORT and LAUNCH_NICKNAME. */
[[nodiscard]] rtype::client::ServerChoice launchChoice();

/** @returns The server at NEXT_MAP_ADDRESS and NEXT_MAP_PORT. */
[[nodiscard]] rtype::network::Endpoint nextMapServer();

/** @returns The status of an attempt the server refused for @p reason. */
[[nodiscard]] rtype::client::ConnectionStatus refusedFor(
    rtype::network::RefusalReason reason);
