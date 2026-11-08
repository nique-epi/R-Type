#include "MasterLink.hpp"
#include <cstdint>
#include <memory>
#include <optional>
#include <stop_token>
#include <thread>
#include <utility>
#include "GameListReading.hpp"
#include "GlobalLeaderboardQuery.hpp"
#include "GlobalLeaderboardReading.hpp"
#include "LatestValue.hpp"
#include "MasterAccess.hpp"
#include "MasterAsks.hpp"
#include "MasterClient.hpp"
#include "MasterReadings.hpp"
#include "NetworkException.hpp"
#include "ServerListConstants.hpp"
#include "ServerListReading.hpp"

namespace rtype::client {

namespace {

/** @returns What @p call reads, or nothing when the master does not answer,
 * refuses, or answers what cannot be read. */
template <typename Call>
auto listingOrNothing(Call call) -> std::optional<decltype(call())> {
  try {
    return call();
  } catch (const network::NetworkException&) {
    return std::nullopt;
  }
}

/**
 * @returns A client of the newest master @p access names when it is newer than
 * revision @p seen, which then moves to that revision; @p current otherwise,
 * and when that master is a url MasterClient refuses.
 */
std::unique_ptr<network::MasterClient> clientFor(
    const concurrency::LatestValue<network::MasterAccess>& access,
    std::uint64_t& seen, std::unique_ptr<network::MasterClient> current) {
  const std::uint64_t revision = access.revision();
  const std::optional<network::MasterAccess> newest = access.value();
  if (revision <= seen || !newest.has_value()) {
    return current;
  }
  seen = revision;
  try {
    return std::make_unique<network::MasterClient>(*newest);
  } catch (const network::InvalidMasterUrlException&) {
    return current;
  }
}

/** @brief Answers the ask for servers of @p asks with @p client when it is
 * newer than the one @p readings served. */
void serveServers(network::MasterClient& client, const MasterAsks& asks,
                  MasterReadings& readings) {
  if (asks.serverAsk <= readings.served.serverAsk) {
    return;
  }
  readings.servers.publish(ServerListReading{
      .ask = asks.serverAsk,
      .listing = listingOrNothing([&client] { return client.listServers(); })});
  readings.served.serverAsk = asks.serverAsk;
}

/** @brief Answers the ask for games of @p asks with @p client when it is
 * newer than the one @p readings served. */
void serveGames(network::MasterClient& client, const MasterAsks& asks,
                MasterReadings& readings) {
  if (asks.gameAsk <= readings.served.gameAsk) {
    return;
  }
  readings.games.publish(GameListReading{
      .ask = asks.gameAsk, .listing = listingOrNothing([&client, &asks] {
                             return client.listGames(asks.gameServerId);
                           })});
  readings.served.gameAsk = asks.gameAsk;
}

/** @brief Answers the ask for the leaderboard of @p asks with @p client when
 * it is newer than the one @p readings served. */
void serveLeaderboard(network::MasterClient& client, const MasterAsks& asks,
                      MasterReadings& readings) {
  if (asks.leaderboardAsk <= readings.served.leaderboardAsk) {
    return;
  }
  readings.leaderboard.publish(GlobalLeaderboardReading{
      .ask = asks.leaderboardAsk,
      .leaderboard = listingOrNothing([&client, &asks] {
        return client.readLeaderboard(asks.leaderboardQuery);
      })});
  readings.served.leaderboardAsk = asks.leaderboardAsk;
}

}  // namespace

void MasterLink::start(network::MasterAccess access) {
  auto client = std::make_unique<network::MasterClient>(std::move(access));
  thread_ = std::jthread(
      [this, client = std::move(client)](const std::stop_token& stop) mutable {
        run(stop, std::move(client));
      });
}

void MasterLink::retarget(network::MasterAccess access) {
  access_.publish(std::move(access));
}

void MasterLink::stop() { thread_ = std::jthread{}; }

std::uint64_t MasterLink::askServers() {
  ++asked_.serverAsk;
  asks_.publish(asked_);
  return asked_.serverAsk;
}

std::uint64_t MasterLink::askGames(std::int64_t serverId) {
  ++asked_.gameAsk;
  asked_.gameServerId = serverId;
  asks_.publish(asked_);
  return asked_.gameAsk;
}

std::uint64_t MasterLink::askLeaderboard(
    const network::GlobalLeaderboardQuery& query) {
  ++asked_.leaderboardAsk;
  asked_.leaderboardQuery = query;
  asks_.publish(asked_);
  return asked_.leaderboardAsk;
}

std::optional<ServerListReading> MasterLink::servers() const {
  return readings_.servers.value();
}

std::optional<GameListReading> MasterLink::games() const {
  return readings_.games.value();
}

std::optional<GlobalLeaderboardReading> MasterLink::leaderboard() const {
  return readings_.leaderboard.value();
}

void MasterLink::run(const std::stop_token& stop,
                     std::unique_ptr<network::MasterClient> client) {
  std::uint64_t seen = 0;
  std::uint64_t accessSeen = 0;
  while (!stop.stop_requested()) {
    seen = asks_.waitForNewer(seen, MASTER_ASK_WAIT, stop);
    client = clientFor(access_, accessSeen, std::move(client));
    const std::optional<MasterAsks> asks = asks_.value();
    if (asks.has_value()) {
      serveServers(*client, *asks, readings_);
      serveGames(*client, *asks, readings_);
      serveLeaderboard(*client, *asks, readings_);
    }
  }
}

}  // namespace rtype::client
