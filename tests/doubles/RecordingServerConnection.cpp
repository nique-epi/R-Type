#include "RecordingServerConnection.hpp"
#include <optional>
#include <ostream>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "ConnectionStatus.hpp"
#include "Endpoint.hpp"
#include "LobbyState.hpp"

void PrintTo(const ServerConnectionCall& call, std::ostream* output) {
  *output << "request " << static_cast<int>(call.request);
  if (call.request == ServerConnectionCall::Request::Connect) {
    *output << " to " << call.server.address << " port " << call.server.port
            << " as " << call.nickname;
  }
  if (call.request == ServerConnectionCall::Request::SetReady) {
    *output << " ready " << call.ready;
  }
}

void RecordingServerConnection::connect(const rtype::network::Endpoint& server,
                                        std::string_view nickname) {
  calls_.push_back({.request = ServerConnectionCall::Request::Connect,
                    .server = server,
                    .nickname = std::string(nickname)});
}

void RecordingServerConnection::cancel() {
  calls_.push_back({.request = ServerConnectionCall::Request::Cancel,
                    .server = {},
                    .nickname = {}});
  status_ = {};
  lobbyPlayers_.clear();
  requestedReady_.reset();
  gameStarted_ = false;
}

rtype::client::ConnectionStatus RecordingServerConnection::status() const {
  return status_;
}

std::span<const rtype::network::LobbyPlayer>
RecordingServerConnection::lobbyPlayers() const {
  return lobbyPlayers_;
}

void RecordingServerConnection::setReady(bool ready) {
  calls_.push_back({.request = ServerConnectionCall::Request::SetReady,
                    .server = {},
                    .nickname = {},
                    .ready = ready});
  requestedReady_ = ready;
}

std::optional<bool> RecordingServerConnection::requestedReady() const {
  return requestedReady_;
}

bool RecordingServerConnection::gameStarted() const { return gameStarted_; }

void RecordingServerConnection::setStatus(
    rtype::client::ConnectionStatus status) {
  status_ = status;
}

void RecordingServerConnection::setLobbyPlayers(
    std::vector<rtype::network::LobbyPlayer> players) {
  lobbyPlayers_ = std::move(players);
}

void RecordingServerConnection::setRequestedReady(std::optional<bool> ready) {
  requestedReady_ = ready;
}

void RecordingServerConnection::setGameStarted(bool started) {
  gameStarted_ = started;
}

const std::vector<ServerConnectionCall>& RecordingServerConnection::calls()
    const {
  return calls_;
}
