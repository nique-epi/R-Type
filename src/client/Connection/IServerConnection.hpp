#pragma once

#include <optional>
#include <span>
#include <string_view>
#include "ConnectionStatus.hpp"
#include "Endpoint.hpp"
#include "LobbyState.hpp"

namespace rtype::client {

/**
 * @brief The attempt of the client to join a server, then the session the
 * server opens, as the screens drive them: start an attempt, drop it, read how
 * it stands; then read who is in the waiting room, ask to be ready or not, and
 * learn that the game started.
 *
 * The screens read it at each of their updates, so an implementation may change
 * any of it at any time between two updates.
 */
class IServerConnection {
 public:
  virtual ~IServerConnection() = default;

  /**
   * @brief Starts an attempt to join a server as a player, in place of any
   * attempt in progress; a session already accepted is left first, as
   * cancel() leaves it.
   *
   * @param nickname 1 to MAXIMUM_NICKNAME_SIZE bytes of UTF-8, as the protocol
   * requires.
   */
  virtual void connect(const network::Endpoint& server,
                       std::string_view nickname) = 0;

  /**
   * @brief Drops the attempt, wherever it stands: the status goes back to
   * ConnectionStatus::Phase::Idle. A session the server accepted is left: the
   * server is told, and the waiting room, the ready state asked for and the
   * start of the game are forgotten.
   */
  virtual void cancel() = 0;

  /** @returns How the attempt stands. */
  [[nodiscard]] virtual ConnectionStatus status() const = 0;

  /**
   * @returns The players of the waiting room as the newest LobbyState of the
   * session lists them: the accepted player alone, not ready, until the first
   * one arrives; empty without a session. Valid until the next update, or the
   * next call to connect() or cancel().
   */
  [[nodiscard]] virtual std::span<const network::LobbyPlayer> lobbyPlayers()
      const = 0;

  /**
   * @brief Asks the server to set the player ready, or not ready: a value,
   * never a toggle, sent again until the waiting room shows it. Ignored
   * without a session, or once the game started.
   */
  virtual void setReady(bool ready) = 0;

  /** @returns The ready state the player last asked for in this session;
   * nothing before any request, and once the game started. */
  [[nodiscard]] virtual std::optional<bool> requestedReady() const = 0;

  /** @returns Whether the server announced the start of the game in this
   * session. */
  [[nodiscard]] virtual bool gameStarted() const = 0;
};

}  // namespace rtype::client
