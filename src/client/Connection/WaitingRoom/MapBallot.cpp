#include "MapBallot.hpp"
#include <algorithm>
#include <cstdint>
#include <optional>
#include <span>
#include "ConnectionConstants.hpp"
#include "ISessionOutbox.hpp"
#include "IWaitingRoom.hpp"
#include "LobbyState.hpp"
#include "MapVote.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The vote the line of the player numbered @p number in @p players
 * shows; nothing when the room does not list that player. */
std::optional<std::uint8_t> shownVoteOf(
    std::span<const network::LobbyPlayer> players, std::uint8_t number) {
  const auto own =
      std::ranges::find(players, number, &network::LobbyPlayer::playerNumber);
  if (own == players.end()) {
    return std::nullopt;
  }
  return own->mapVote;
}

}  // namespace

MapBallot::MapBallot(const IWaitingRoom& room) : room_(&room) {}

void MapBallot::voteFor(std::uint8_t map) {
  if (outbox_ == nullptr || room_->gameStarted()) {
    return;
  }
  requestedMap_ = map;
  sendMessage(*outbox_, network::MapVote{.map = map});
  sinceVote_ = engine::Duration::zero();
}

std::optional<std::uint8_t> MapBallot::requestedMap() const {
  return requestedMap_;
}

void MapBallot::sessionOpened(const network::LobbyPlayer& ownPlayer,
                              ISessionOutbox& outbox) {
  outbox_ = &outbox;
  ownPlayerNumber_ = ownPlayer.playerNumber;
}

void MapBallot::sessionClosed() {
  outbox_ = nullptr;
  requestedMap_.reset();
  sinceVote_ = engine::Duration::zero();
}

void MapBallot::sessionHeld(engine::Duration elapsed) {
  if (outbox_ == nullptr || !requestedMap_.has_value() ||
      room_->gameStarted() ||
      shownVoteOf(room_->lobbyPlayers(), ownPlayerNumber_) == requestedMap_) {
    return;
  }
  sinceVote_ += elapsed;
  if (sinceVote_ >= MAP_VOTE_INTERVAL) {
    sendMessage(*outbox_, network::MapVote{.map = *requestedMap_});
    sinceVote_ = engine::Duration::zero();
  }
}

}  // namespace rtype::client
