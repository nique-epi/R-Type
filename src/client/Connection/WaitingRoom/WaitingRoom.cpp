#include "WaitingRoom.hpp"
#include <algorithm>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <vector>
#include "ConnectionConstants.hpp"
#include "GameStarted.hpp"
#include "ISessionOutbox.hpp"
#include "LobbyState.hpp"
#include "MessageDispatcher.hpp"
#include "MessageOrigin.hpp"
#include "ReadyState.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns Whether the line of the player numbered @p number in @p players
 * shows @p ready. */
bool roomShowsReady(std::span<const network::LobbyPlayer> players,
                    std::uint8_t number, bool ready) {
  const auto own =
      std::ranges::find(players, number, &network::LobbyPlayer::playerNumber);
  return own != players.end() && own->ready == ready;
}

}  // namespace

WaitingRoom::WaitingRoom(network::MessageDispatcher& dispatcher)
    : logger_(std::string{CONNECTION_LOGGER_NAME}) {
  dispatcher.registerHandler<network::LobbyState>(
      [this](const network::MessageOrigin&, const network::LobbyState& lobby) {
        if (outbox_ != nullptr) {
          players_ = lobby.players;
        }
      });
  dispatcher.registerHandler<network::GameStarted>(
      [this](const network::MessageOrigin&, const network::GameStarted&) {
        startGame();
      });
}

std::span<const network::LobbyPlayer> WaitingRoom::lobbyPlayers() const {
  return players_;
}

void WaitingRoom::setReady(bool ready) {
  if (outbox_ == nullptr || gameStarted_) {
    return;
  }
  requestedReady_ = ready;
  sendReadyState(ready);
}

std::optional<bool> WaitingRoom::requestedReady() const {
  return requestedReady_;
}

bool WaitingRoom::gameStarted() const { return gameStarted_; }

void WaitingRoom::sessionOpened(const network::LobbyPlayer& ownPlayer,
                                ISessionOutbox& outbox) {
  outbox_ = &outbox;
  ownPlayerNumber_ = ownPlayer.playerNumber;
  players_ = {ownPlayer};
}

void WaitingRoom::sessionClosed() {
  outbox_ = nullptr;
  players_.clear();
  requestedReady_.reset();
  sinceReadyState_ = engine::Duration::zero();
  gameStarted_ = false;
}

void WaitingRoom::sessionHeld(engine::Duration elapsed) {
  if (!requestedReady_.has_value() ||
      roomShowsReady(players_, ownPlayerNumber_, *requestedReady_)) {
    return;
  }
  sinceReadyState_ += elapsed;
  if (sinceReadyState_ >= READY_STATE_INTERVAL) {
    sendReadyState(*requestedReady_);
  }
}

void WaitingRoom::startGame() {
  if (outbox_ != nullptr && !gameStarted_) {
    gameStarted_ = true;
    requestedReady_.reset();
    logger_.info("the game started");
  }
}

void WaitingRoom::endGame() {
  gameStarted_ = false;
  logger_.info("the game ended");
}

void WaitingRoom::sendReadyState(bool ready) {
  sendMessage(*outbox_, network::ReadyState{.ready = ready});
  sinceReadyState_ = engine::Duration::zero();
}

}  // namespace rtype::client
