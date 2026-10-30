#pragma once

#include "ISessionOutbox.hpp"
#include "LobbyState.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief A part of the client that follows the session with the server: it
 * learns when the session opens and closes, and gets a turn in each frame the
 * session holds.
 */
class ISessionObserver {
 public:
  virtual ~ISessionObserver() = default;

  /**
   * @brief The server accepted the player: the session is open.
   * @param ownPlayer The player the server accepted, alone in the waiting
   * room until the first LobbyState.
   * @param outbox Where the observer sends to the server; it stays valid
   * until sessionClosed().
   */
  virtual void sessionOpened(const network::LobbyPlayer& ownPlayer,
                             ISessionOutbox& outbox) = 0;

  /** @brief The session is over, left, lost or never opened: forget it. */
  virtual void sessionClosed() = 0;

  /**
   * @brief One frame of an open session went by, after the datagrams of the
   * server were read and before the next Heartbeat is due.
   * @param elapsed Time since the previous frame.
   */
  virtual void sessionHeld(engine::Duration elapsed) = 0;
};

}  // namespace rtype::client
