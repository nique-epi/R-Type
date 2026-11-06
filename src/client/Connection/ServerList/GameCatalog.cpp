#include "GameCatalog.hpp"
#include <algorithm>
#include <cstdint>
#include <optional>
#include <utility>
#include <vector>
#include "GameListReading.hpp"
#include "GameListSnapshot.hpp"
#include "GameListing.hpp"
#include "GameRow.hpp"
#include "IMasterReads.hpp"
#include "ListedGame.hpp"
#include "ListedGameState.hpp"
#include "ServerListPhase.hpp"
#include "ServerRow.hpp"

namespace rtype::client {

namespace {

/** @returns Whether @p first comes before @p second in the panel: a game a
 * player can join first, then the lower number. */
bool comesBefore(const GameRow& first, const GameRow& second) {
  if (isJoinable(first) != isJoinable(second)) {
    return isJoinable(first);
  }
  return first.number < second.number;
}

/** @returns The rows of the games of @p listing, in the order of the panel. */
std::vector<GameRow> rowsOf(const network::GameListing& listing,
                            std::uint8_t maximumPlayers) {
  std::vector<GameRow> rows;
  rows.reserve(listing.games.size());
  for (const network::ListedGame& game : listing.games) {
    rows.push_back(GameRow{.number = game.number,
                           .name = game.name,
                           .state = game.state,
                           .playerCount = game.playerCount,
                           .maximumPlayers = maximumPlayers});
  }
  std::ranges::sort(rows, comesBefore);
  return rows;
}

}  // namespace

bool isJoinable(const GameRow& game) {
  return game.state == network::ListedGameState::Waiting;
}

GameCatalog::GameCatalog(IMasterReads& master) : master_(&master) {}

void GameCatalog::ask(const ServerRow& server) {
  asked_ = master_->askGames(server.id);
  maximumPlayers_ = server.maximumPlayersPerGame;
  snapshot_ =
      GameListSnapshot{.phase = ServerListPhase::Searching, .games = {}};
}

const GameListSnapshot& GameCatalog::snapshot() const { return snapshot_; }

void GameCatalog::update() {
  const std::optional<GameListReading> reading = master_->games();
  if (!reading.has_value() || reading->ask != asked_ || taken_ == asked_) {
    return;
  }
  taken_ = asked_;
  if (!reading->listing.has_value()) {
    snapshot_ = GameListSnapshot{.phase = ServerListPhase::MasterUnreachable,
                                 .games = {}};
    return;
  }
  std::vector<GameRow> rows = rowsOf(*reading->listing, maximumPlayers_);
  snapshot_ = GameListSnapshot{
      .phase = rows.empty() ? ServerListPhase::Empty : ServerListPhase::Ready,
      .games = std::move(rows)};
}

}  // namespace rtype::client
