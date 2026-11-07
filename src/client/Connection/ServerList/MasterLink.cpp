#include "MasterLink.hpp"
#include <cstdint>
#include <memory>
#include <optional>
#include <stop_token>
#include <thread>
#include <utility>
#include "GameListReading.hpp"
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

}  // namespace

void MasterLink::start(network::MasterAccess access) {
  client_ = std::make_unique<network::MasterClient>(std::move(access));
  thread_ = std::jthread([this](const std::stop_token& stop) { run(stop); });
}

void MasterLink::stop() {
  thread_ = std::jthread{};
  client_.reset();
}

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

void MasterLink::run(const std::stop_token& stop) {
  std::uint64_t seen = 0;
  MasterAsks served;
  while (!stop.stop_requested()) {
    seen = asks_.waitForNewer(seen, MASTER_ASK_WAIT, stop);
    if (const std::optional<MasterAsks> asks = asks_.value()) {
      serve(*asks, served);
    }
  }
}

void MasterLink::serve(const MasterAsks& asks, MasterAsks& served) {
  if (asks.serverAsk > served.serverAsk) {
    servers_.publish(ServerListReading{.ask = asks.serverAsk,
                                       .listing = listingOrNothing([this] {
                                         return client_->listServers();
                                       })});
    served.serverAsk = asks.serverAsk;
  }
  if (asks.gameAsk > served.gameAsk) {
    games_.publish(GameListReading{
        .ask = asks.gameAsk, .listing = listingOrNothing([this, &asks] {
                               return client_->listGames(asks.gameServerId);
                             })});
    served.gameAsk = asks.gameAsk;
  }
}

}  // namespace rtype::client
