#pragma once

#include <vector>
#include "RecordingServerConnection.hpp"

/** @returns The requests to start an attempt the connection received, in
 * order, without the cancels. */
[[nodiscard]] std::vector<ServerConnectionCall> connectAttemptsOf(
    const RecordingServerConnection& connection);
