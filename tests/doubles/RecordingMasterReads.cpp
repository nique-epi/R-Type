#include "RecordingMasterReads.hpp"
#include <cstdint>
#include <optional>
#include <utility>
#include <vector>
#include "GameListReading.hpp"
#include "GameListing.hpp"
#include "ServerListReading.hpp"
#include "ServerListing.hpp"

std::uint64_t RecordingMasterReads::askServers() { return ++serverAsk_; }

std::uint64_t RecordingMasterReads::askGames(std::int64_t serverId) {
  gameAsks_.push_back(serverId);
  return gameAsks_.size();
}

std::optional<rtype::client::ServerListReading> RecordingMasterReads::servers()
    const {
  return servers_;
}

std::optional<rtype::client::GameListReading> RecordingMasterReads::games()
    const {
  return games_;
}

void RecordingMasterReads::answerServers(
    std::optional<rtype::network::ServerListing> listing) {
  servers_ = rtype::client::ServerListReading{.ask = serverAsk_,
                                              .listing = std::move(listing)};
}

void RecordingMasterReads::answerGames(
    std::uint64_t ask, std::optional<rtype::network::GameListing> listing) {
  games_ =
      rtype::client::GameListReading{.ask = ask, .listing = std::move(listing)};
}

std::uint64_t RecordingMasterReads::serverAskCount() const {
  return serverAsk_;
}

const std::vector<std::int64_t>& RecordingMasterReads::gameAsks() const {
  return gameAsks_;
}
