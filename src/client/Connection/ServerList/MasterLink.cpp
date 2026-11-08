#include "MasterLink.hpp"
#include <cstdint>
#include <memory>
#include <optional>
#include <stop_token>
#include <thread>
#include <utility>
#include "GameListReading.hpp"
#include "LatestValue.hpp"
#include "MasterAccess.hpp"
#include "MasterAsks.hpp"
#include "MasterClient.hpp"
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

std::optional<ServerListReading> MasterLink::servers() const {
  return servers_.value();
}

std::optional<GameListReading> MasterLink::games() const {
  return games_.value();
}

void MasterLink::run(const std::stop_token& stop,
                     std::unique_ptr<network::MasterClient> client) {
  std::uint64_t seen = 0;
  std::uint64_t accessSeen = 0;
  MasterAsks served;
  while (!stop.stop_requested()) {
    seen = asks_.waitForNewer(seen, MASTER_ASK_WAIT, stop);
    client = clientFor(access_, accessSeen, std::move(client));
    const std::optional<MasterAsks> asks = asks_.value();
    if (asks.has_value()) {
      serve(*client, *asks, served);
    }
  }
}

void MasterLink::serve(network::MasterClient& client, const MasterAsks& asks,
                       MasterAsks& served) {
  if (asks.serverAsk > served.serverAsk) {
    servers_.publish(ServerListReading{.ask = asks.serverAsk,
                                       .listing = listingOrNothing([&client] {
                                         return client.listServers();
                                       })});
    served.serverAsk = asks.serverAsk;
  }
  if (asks.gameAsk > served.gameAsk) {
    games_.publish(GameListReading{
        .ask = asks.gameAsk, .listing = listingOrNothing([&client, &asks] {
                               return client.listGames(asks.gameServerId);
                             })});
    served.gameAsk = asks.gameAsk;
  }
}

}  // namespace rtype::client
