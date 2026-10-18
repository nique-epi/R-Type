#pragma once

#include <string_view>
#include "ConnectionStatus.hpp"
#include "Endpoint.hpp"

namespace rtype::client {

/**
 * @brief The attempt of the client to join a server, as the connection screen
 * drives it: start one, drop it, and read how it stands.
 *
 * The screen reads status() at each of its updates, so an implementation may
 * move from one phase to another at any time between two updates.
 */
class IServerConnection {
 public:
  virtual ~IServerConnection() = default;

  /**
   * @brief Starts an attempt to join a server as a player, in place of any
   * attempt in progress.
   *
   * @param nickname 1 to MAXIMUM_NICKNAME_SIZE bytes of UTF-8, as the protocol
   * requires.
   */
  virtual void connect(const network::Endpoint& server,
                       std::string_view nickname) = 0;

  /** @brief Drops the attempt, wherever it stands: the status goes back to
   * ConnectionStatus::Phase::Idle. */
  virtual void cancel() = 0;

  /** @returns How the attempt stands. */
  [[nodiscard]] virtual ConnectionStatus status() const = 0;
};

}  // namespace rtype::client
