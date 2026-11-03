#pragma once

#include <optional>
#include <span>
#include "LobbyState.hpp"

namespace rtype::client {

/**
 * @brief The waiting room of the session, as the screens read and drive it:
 * who is in it, the ready state the player asked for, and whether the game
 * started.
 *
 * The screens read it at each of their updates, so an implementation may change
 * any of it at any time between two updates.
 */
class IWaitingRoom {
 public:
  virtual ~IWaitingRoom() = default;

  /**
   * @returns The players of the waiting room as the newest LobbyState of the
   * session lists them: the accepted player alone, not ready, until the first
   * one arrives; empty without a session. Valid until the next update, or the
   * next call to connect() or cancel() of the connection.
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
