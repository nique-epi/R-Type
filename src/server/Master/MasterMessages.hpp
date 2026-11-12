#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include "FinishedGame.hpp"
#include "GameResult.hpp"
#include "HostedGame.hpp"
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
 * network::MAXIMUM_PLAYERS per game, its probe secret and the address of the
 * identity; without one, the master takes the address the request comes from.
 */
[[nodiscard]] network::ServerRegistration registrationOf(
    const ServerIdentity& identity);

/**
 * @returns The heartbeat of @p games, with @p measures and with @p probeToken
 * when a probe came since the last heartbeat sent.
 */
[[nodiscard]] network::ServerHeartbeat heartbeatOf(
    std::vector<network::HostedGame> games,
    const network::ServerMeasures& measures,
    std::optional<std::uint64_t> probeToken);

/**
 * @returns @p result as the master keeps it: its date, its difficulty, its
 * players when it started, whether it was vanilla, and the nickname and score
 * of each player, a guest each; the deaths stay on this server.
 */
[[nodiscard]] network::FinishedGame finishedGameOf(const GameResult& result);

}  // namespace rtype::server
