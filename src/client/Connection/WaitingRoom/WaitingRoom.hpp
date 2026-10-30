#pragma once

#include <cstdint>
#include <optional>
#include <span>
#include <vector>
#include "ISessionObserver.hpp"
#include "ISessionOutbox.hpp"
#include "IWaitingRoom.hpp"
#include "LobbyState.hpp"
#include "Logger.hpp"
#include "MessageDispatcher.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The waiting room of the session, as the server shows it. It keeps the
 * players of the newest LobbyState, and learns that the game started from a
 * GameStarted, or from a WorldState when the GameStarted was lost (see
 * EntityMessageQueue).
 *
 * setReady() sends a ReadyState at once, then again every READY_STATE_INTERVAL
 * while the waiting room shows another value. A game that ended is no longer
 * started: the waiting room can be entered again, and ready asked again.
 *
 * It lives from the acceptance of the player to the end of the session, and is
 * empty outside it.
 */
class WaitingRoom final : public IWaitingRoom, public ISessionObserver {
 public:
  /** @param dispatcher Where the handlers of LobbyState and GameStarted are
   * registered; it must outlive the room. */
  explicit WaitingRoom(network::MessageDispatcher& dispatcher);

  [[nodiscard]] std::span<const network::LobbyPlayer> lobbyPlayers()
      const override;
  void setReady(bool ready) override;
  [[nodiscard]] std::optional<bool> requestedReady() const override;
  [[nodiscard]] bool gameStarted() const override;

  /** @brief The room holds the accepted player alone, not ready, and sends
   * through @p outbox. */
  void sessionOpened(const network::LobbyPlayer& ownPlayer,
                     ISessionOutbox& outbox) override;

  /** @brief Forgets the players, the ready state asked for and the start of
   * the game. */
  void sessionClosed() override;

  /** @brief Sends the ReadyState again once READY_STATE_INTERVAL passed while
   * the waiting room shows another value than the one asked for. */
  void sessionHeld(engine::Duration elapsed) override;

  /** @brief Notes that the game started, once per session, and forgets the
   * ready state asked for, which belonged to the waiting room. Does nothing
   * outside a session. */
  void startGame();

  /** @brief Notes that the game ended: the waiting room can be entered
   * again. */
  void endGame();

 private:
  void sendReadyState(bool ready);

  logging::Logger logger_;
  ISessionOutbox* outbox_{nullptr};
  std::vector<network::LobbyPlayer> players_;
  std::uint8_t ownPlayerNumber_{0};
  std::optional<bool> requestedReady_;
  bool gameStarted_{false};
  engine::Duration sinceReadyState_{};
};

}  // namespace rtype::client
