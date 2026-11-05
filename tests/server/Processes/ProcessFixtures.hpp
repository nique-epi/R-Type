#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include "IChildProcess.hpp"
#include "ProcessExit.hpp"

/** @returns How @p child ended, polling it until @p timeout; nothing when it
 * still runs then. */
[[nodiscard]] std::optional<rtype::server::ProcessExit> waitForExit(
    rtype::server::IChildProcess& child, std::chrono::milliseconds timeout);

/** @returns Whether the process @p processId is running now: a process that
 * ended and waits to be collected does not count. */
[[nodiscard]] bool processIsRunning(std::int64_t processId);

/** @returns The process id written in the file @p path, polling until
 * @p timeout; nothing when the file stays empty. */
[[nodiscard]] std::optional<std::int64_t> waitForProcessIdIn(
    const std::string& path, std::chrono::milliseconds timeout);

/** @returns Whether the process @p processId ended before @p timeout. */
[[nodiscard]] bool waitForProcessToEnd(std::int64_t processId,
                                       std::chrono::milliseconds timeout);
