#pragma once

#include <cstdint>
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
 * @returns A secret to register with, drawn from std::random_device between
 * network::LOWEST_PROBE_SECRET and network::HIGHEST_PROBE_SECRET.
 */
[[nodiscard]] std::uint64_t randomProbeSecret();

/**
 * @returns The registration of the server of @p identity: its name, its port,
 * SERVER_GAME_VERSION, network::PROTOCOL_VERSION, its games,
 * network::MAXIMUM_PLAYERS per game and its probe secret. It names no address,
 * so the master takes the address the request comes from.
 */
[[nodiscard]] network::ServerRegistration registrationOf(
    const ServerIdentity& identity);

/**
 * @returns The measures of @p report the master reads: the median and 95th
 * percentile of the ticks, and the mean round trip of the players; a measure
 * the report has not taken stays empty.
 */
[[nodiscard]] network::ServerMeasures measuresOf(const MetricsReport& report);

/**
 * @returns The heartbeat of @p games, with the measures of @p report when
 * there is one, and @p probeToken when a probe came since the last heartbeat
 * sent.
 */
[[nodiscard]] network::ServerHeartbeat heartbeatOf(
    std::vector<network::HostedGame> games,
    const std::optional<MetricsReport>& report,
    std::optional<std::uint64_t> probeToken);

}  // namespace rtype::server
