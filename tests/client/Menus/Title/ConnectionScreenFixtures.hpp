#pragma once

#include <vector>
#include "RecordingServerConnection.hpp"
#include "ServerChoice.hpp"

/** @returns The requests to start an attempt the connection received, in
 * order, without the cancels. */
[[nodiscard]] std::vector<ServerConnectionCall> connectAttemptsOf(
    const RecordingServerConnection& connection);

/** @returns The choice of server the launch options give the tests:
 * LAUNCH_ADDRESS, LAUNCH_PORT and LAUNCH_NICKNAME. */
[[nodiscard]] rtype::client::ServerChoice launchChoice();
