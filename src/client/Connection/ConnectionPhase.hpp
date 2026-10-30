#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief The steps of an attempt to join a server. */
enum class ConnectionPhase : std::uint8_t {
  /** @brief No attempt: none started, or the last one dropped. */
  Idle,
  /** @brief The request goes out again and again until an answer comes. */
  Connecting,
  /**
   * @brief The server gave the player a place: playerNumber holds its
   * number.
   */
  Accepted,
  /** @brief The server refused: refusal holds why. */
  Refused,
  /** @brief No answer came before the client gave up. */
  Unreachable,
  /**
   * @brief The server accepted the player, then sent nothing that passed
   * the checks of the session for SESSION_SILENCE_LIMIT: the client closed the
   * session without telling the server.
   */
  Lost,
  /**
   * @brief The server said within the session that it shuts down: a
   * Disconnection carrying the token of the session arrived, and the client
   * closed the session without answering.
   */
  Closed,
};

}  // namespace rtype::client
