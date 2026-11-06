#pragma once

#include <optional>
#include <vector>
#include "HostedGame.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerIdentity.hpp"
#include "ServerMeasures.hpp"
#include "ServerRegistration.hpp"

namespace rtype::server {

/**
 * @returns The registration of the server of @p identity: its name, its port,
 * SERVER_GAME_VERSION, network::PROTOCOL_VERSION, its games, and
 * network::MAXIMUM_PLAYERS per game. It names no address, so the master takes
 * the address the request comes from.
 */
[[nodiscard]] network::ServerRegistration registrationOf(
    const ServerIdentity& identity);

/** @returns The heartbeat of @p games, with @p measures when there are some,
 * and no measure otherwise. */
[[nodiscard]] network::ServerHeartbeat heartbeatOf(
    std::vector<network::HostedGame> games,
    const std::optional<network::ServerMeasures>& measures);

}  // namespace rtype::server
