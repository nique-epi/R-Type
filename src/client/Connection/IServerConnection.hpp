#pragma once

#include <string_view>
#include "ConnectionStatus.hpp"
#include "Endpoint.hpp"

namespace rtype::client {

/**
 * @brief The attempt of the client to join a server, as the screens drive it:
 * start an attempt, drop it, read how it stands. The waiting room of the
 * session it opens is IWaitingRoom.
 *
 * The screens read it at each of their updates, so an implementation may change
 * it at any time between two updates.
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

  /** @returns How the attempt stands: ConnectionStatus::Phase::Lost once the
   * server of a session went silent for SESSION_TIMEOUT, until the next
   * connect() or cancel(). */
  [[nodiscard]] virtual ConnectionStatus status() const = 0;
};

}  // namespace rtype::client
