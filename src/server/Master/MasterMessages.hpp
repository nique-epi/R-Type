#pragma once

#include <optional>
#include <vector>
#include "HostedGame.hpp"
#include "MetricsReport.hpp"
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

/**
 * @returns The measures of @p report the master reads: the median and 95th
 * percentile of the ticks, and the mean round trip of the players; a measure
 * the report has not taken stays empty.
 */
[[nodiscard]] network::ServerMeasures measuresOf(const MetricsReport& report);

/** @returns The heartbeat of @p games, with the measures of @p report when
 * there is one. */
[[nodiscard]] network::ServerHeartbeat heartbeatOf(
    std::vector<network::HostedGame> games,
    const std::optional<MetricsReport>& report);

}  // namespace rtype::server
