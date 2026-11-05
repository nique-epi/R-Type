#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string_view>
#include "KnownServer.hpp"
#include "ServerStatus.hpp"
#include "ServersService.hpp"

/** @returns A server created as "Paris 1" from @p address at @p date, then
 * registered as parisRegistration() at that date. */
[[nodiscard]] rtype::master::KnownServer registeredParis(
    rtype::master::ServersService& servers, std::string_view address,
    std::chrono::sys_seconds date);

/** @returns The status the list of @p servers gives @p serverId, nothing when
 * the list leaves it out. */
[[nodiscard]] std::optional<rtype::network::ServerStatus> listedStatusOf(
    rtype::master::ServersService& servers, std::int64_t serverId);
