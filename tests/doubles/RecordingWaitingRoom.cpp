#include "RecordingWaitingRoom.hpp"
#include <optional>
#include <span>
#include <utility>
#include <vector>
#include "LobbyState.hpp"

std::span<const rtype::network::LobbyPlayer>
RecordingWaitingRoom::lobbyPlayers() const {
  return lobbyPlayers_;
}

void RecordingWaitingRoom::setReady(bool ready) {
  readyRequests_.push_back(ready);
  requestedReady_ = ready;
}

std::optional<bool> RecordingWaitingRoom::requestedReady() const {
  return requestedReady_;
}

bool RecordingWaitingRoom::gameStarted() const { return gameStarted_; }

void RecordingWaitingRoom::setLobbyPlayers(
    std::vector<rtype::network::LobbyPlayer> players) {
  lobbyPlayers_ = std::move(players);
}

void RecordingWaitingRoom::setRequestedReady(std::optional<bool> ready) {
  requestedReady_ = ready;
}

void RecordingWaitingRoom::setGameStarted(bool started) {
  gameStarted_ = started;
}

const std::vector<bool>& RecordingWaitingRoom::readyRequests() const {
  return readyRequests_;
}
