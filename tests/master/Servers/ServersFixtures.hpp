#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "KnownServer.hpp"
#include "MasterDatabase.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerStatus.hpp"
#include "ServersService.hpp"

/** @returns The key of a server created as "Paris 1" from @p address at
 * @p date, then registered as parisRegistration() at that date. */
[[nodiscard]] std::string keyOfRegisteredParis(
    rtype::master::ServersService& servers, std::string_view address,
    std::chrono::sys_seconds date);

/** @returns The server keyOfRegisteredParis() registers. */
[[nodiscard]] rtype::master::KnownServer registeredParis(
    rtype::master::ServersService& servers, std::string_view address,
    std::chrono::sys_seconds date);

/** @brief Runs @p statement on @p database, as a test that makes it refuse a
 * write does. */
void executeStatement(rtype::master::MasterDatabase& database,
                      std::string_view statement);

/** @returns The status the list of @p servers gives @p serverId, nothing when
 * the list leaves it out. */
[[nodiscard]] std::optional<rtype::network::ServerStatus> listedStatusOf(
    rtype::master::ServersService& servers, std::int64_t serverId);

/**
 * @returns The heartbeat of "Paris 1" that answers the probe @p servers sends
 * @p server at @p date: parisHeartbeat() with the token of that probe.
 */
[[nodiscard]] rtype::network::ServerHeartbeat answeringHeartbeat(
    const rtype::master::ServersService& servers,
    const rtype::master::KnownServer& server, std::chrono::sys_seconds date);
