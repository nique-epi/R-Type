#pragma once

#include "ISessionObserver.hpp"
#include "ISessionOutbox.hpp"
#include "LobbyState.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Tells the server that this client shows the game: from the start of
 * the game until the first WorldState arrives, a GameLoaded goes out before an
 * input, at most one every GAME_LOADED_INTERVAL, so the server starts the game
 * once every client shows it. The first one is due at once.
 *
 * The waiting room starts it, the entity message queue ends the wait, and the
 * end of the game or of the session forgets it.
 */
class GameLoadedReminder final : public ISessionObserver {
 public:
  /** @brief The game started: a GameLoaded is due at once. */
  void begin();

  /** @brief A WorldState arrived: the server needs no more GameLoaded. */
  void worldStateArrived();

  /** @brief The game ended: forgets the start and the WorldState. */
  void end();

  /** @brief Sends a GameLoaded through @p outbox when the game started, no
   * WorldState arrived yet and GAME_LOADED_INTERVAL passed since the previous
   * one. */
  void sendWhenDue(ISessionOutbox& outbox);

  void sessionOpened(const network::LobbyPlayer& ownPlayer,
                     ISessionOutbox& outbox) override;
  void sessionClosed() override;

  /** @brief Counts @p elapsed since the previous GameLoaded. */
  void sessionHeld(engine::Duration elapsed) override;

 private:
  bool started_{false};
  bool worldStateSeen_{false};
  engine::Duration sinceGameLoaded_{};
};

}  // namespace rtype::client
