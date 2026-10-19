#pragma once

#include <cstdint>
#include <optional>
#include <ostream>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "ConnectionStatus.hpp"
#include "Endpoint.hpp"
#include "IServerConnection.hpp"
#include "LobbyState.hpp"

/** @brief What a screen asked of the connection: to start an attempt, with its
 * server and its nickname, to drop it, or to set the player ready or not. */
struct ServerConnectionCall {
  enum class Request : std::uint8_t {
    Connect,
    Cancel,
    SetReady,
  };

  Request request{};
  rtype::network::Endpoint server;
  std::string nickname;
  /** @brief The value SetReady asked for. */
  bool ready = false;

  bool operator==(const ServerConnectionCall& other) const = default;
};

/** @brief Prints a request in test failure messages. */
void PrintTo(const ServerConnectionCall& call, std::ostream* output);

/**
 * @brief Connection double that writes down every request, in order, and whose
 * status, waiting room and start of the game the test sets: a real connection
 * changes them on its own, at any time between two updates.
 */
class RecordingServerConnection : public rtype::client::IServerConnection {
 public:
  /** @brief Writes the request down; the status stays what the test set. */
  void connect(const rtype::network::Endpoint& server,
               std::string_view nickname) override;
  /** @brief Writes the request down, and goes back to no attempt and no
   * session, as a real connection does. */
  void cancel() override;
  [[nodiscard]] rtype::client::ConnectionStatus status() const override;
  [[nodiscard]] std::span<const rtype::network::LobbyPlayer> lobbyPlayers()
      const override;
  /** @brief Writes the request down, and keeps the value as the one asked
   * for, as a real connection does. */
  void setReady(bool ready) override;
  [[nodiscard]] std::optional<bool> requestedReady() const override;
  [[nodiscard]] bool gameStarted() const override;

  void setStatus(rtype::client::ConnectionStatus status);
  void setLobbyPlayers(std::vector<rtype::network::LobbyPlayer> players);
  /** @brief Sets the ready state asked for, as a request made before the test
   * would have, without writing a request down. */
  void setRequestedReady(std::optional<bool> ready);
  void setGameStarted(bool started);
  [[nodiscard]] const std::vector<ServerConnectionCall>& calls() const;

 private:
  rtype::client::ConnectionStatus status_;
  std::vector<rtype::network::LobbyPlayer> lobbyPlayers_;
  std::optional<bool> requestedReady_;
  bool gameStarted_ = false;
  std::vector<ServerConnectionCall> calls_;
};
