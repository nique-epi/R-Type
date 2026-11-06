#include "ServerCatalog.hpp"
#include <algorithm>
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "Endpoint.hpp"
#include "IMasterReads.hpp"
#include "IServerPingProbe.hpp"
#include "ListedServer.hpp"
#include "PingAnswer.hpp"
#include "ProtocolConstants.hpp"
#include "ServerCompatibility.hpp"
#include "ServerListConstants.hpp"
#include "ServerListPhase.hpp"
#include "ServerListReading.hpp"
#include "ServerListSnapshot.hpp"
#include "ServerListing.hpp"
#include "ServerRow.hpp"
#include "ServerStatus.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns Whether a player may be shown @p server: online or degraded, with
 * the address players reach it on. */
bool isListable(const network::ListedServer& server) {
  return (server.status == network::ServerStatus::Online ||
          server.status == network::ServerStatus::Degraded) &&
         server.registration.address.has_value();
}

ServerCompatibility compatibilityOf(const network::ListedServer& server) {
  return server.registration.protocolVersion == network::PROTOCOL_VERSION
             ? ServerCompatibility::Compatible
             : ServerCompatibility::UpdateNeeded;
}

/** @returns The row of @p server, which isListable(), with no ping yet. */
ServerRow rowOf(const network::ListedServer& server) {
  const network::ServerRegistration& registration = server.registration;
  return ServerRow{
      .id = server.id,
      .name = registration.name,
      .endpoint = network::Endpoint{.address = registration.address.value_or(
                                        std::string{}),
                                    .port = registration.udpPort},
      .playerCount = server.playerCount,
      .gameCount = server.gameCount,
      .maximumPlayers = static_cast<std::uint16_t>(
          registration.maximumGames * registration.maximumPlayersPerGame),
      .maximumPlayersPerGame = registration.maximumPlayersPerGame,
      .gameVersion = registration.gameVersion,
      .status = server.status,
      .compatibility = compatibilityOf(server),
      .ping = std::nullopt};
}

/** @returns The rows of the servers of @p listing a player may be shown. */
std::vector<ServerRow> listedRowsOf(const network::ServerListing& listing) {
  std::vector<ServerRow> rows;
  for (const network::ListedServer& server : listing.servers) {
    if (isListable(server)) {
      rows.push_back(rowOf(server));
    }
  }
  return rows;
}

/** @returns Where each of @p rows is reached, in order. */
std::vector<network::Endpoint> endpointsOf(const std::vector<ServerRow>& rows) {
  std::vector<network::Endpoint> endpoints;
  endpoints.reserve(rows.size());
  for (const ServerRow& row : rows) {
    endpoints.push_back(row.endpoint);
  }
  return endpoints;
}

/** @returns Whether @p first comes before @p second in the list: lower ping,
 * a ping before none, then lower id. */
bool comesBefore(const ServerRow& first, const ServerRow& second) {
  if (first.ping.has_value() != second.ping.has_value()) {
    return first.ping.has_value();
  }
  if (first.ping != second.ping) {
    return first.ping < second.ping;
  }
  return first.id < second.id;
}

}  // namespace

ServerCatalog::ServerCatalog(IMasterReads& master, IServerPingProbe& pings)
    : master_(&master), pings_(&pings) {}

void ServerCatalog::refresh() { asked_ = master_->askServers(); }

const ServerListSnapshot& ServerCatalog::snapshot() const { return snapshot_; }

void ServerCatalog::update(engine::Duration elapsed) {
  if (const std::optional<ServerListReading> reading = master_->servers();
      reading.has_value() && reading->ask == asked_ && taken_ != asked_) {
    taken_ = asked_;
    takeListing(*reading);
  }
  if (!roundAge_.has_value()) {
    return;
  }
  const engine::Duration age = *roundAge_ + elapsed;
  roundAge_ = age;
  takeAnswers();
  if (waiting_.empty() || age >= PING_GIVE_UP_TIME) {
    closeRound();
  }
}

void ServerCatalog::takeListing(const ServerListReading& reading) {
  roundAge_.reset();
  waiting_.clear();
  measured_.clear();
  if (!reading.listing.has_value()) {
    snapshot_ = ServerListSnapshot{.phase = ServerListPhase::MasterUnreachable,
                                   .servers = {}};
    return;
  }
  for (ServerRow& row : listedRowsOf(*reading.listing)) {
    (row.compatibility == ServerCompatibility::Compatible ? waiting_
                                                          : measured_)
        .push_back(std::move(row));
  }
  pings_->measure(endpointsOf(waiting_));
  roundAge_ = engine::Duration::zero();
}

void ServerCatalog::takeAnswers() {
  for (const PingAnswer& answer : pings_->answers()) {
    const auto row =
        std::ranges::find(waiting_, answer.server, &ServerRow::endpoint);
    if (row != waiting_.end()) {
      row->ping = answer.roundTrip;
      measured_.push_back(std::move(*row));
      waiting_.erase(row);
    }
  }
}

void ServerCatalog::closeRound() {
  std::ranges::sort(measured_, comesBefore);
  snapshot_ =
      ServerListSnapshot{.phase = measured_.empty() ? ServerListPhase::Empty
                                                    : ServerListPhase::Ready,
                         .servers = std::move(measured_)};
  measured_.clear();
  waiting_.clear();
  roundAge_.reset();
}

}  // namespace rtype::client
