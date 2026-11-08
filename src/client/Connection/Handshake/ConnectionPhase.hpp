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
  /**
   * @brief The server said within the session that the host expelled the
   * player: an Expulsion carrying the token of the session arrived, and the
   * client closed the session without answering.
   */
  Expelled,
  /**
   * @brief The server accepted the player and plays with another content
   * than the client's own, which is not kept: the session holds its seat
   * while the pack downloads, and contentReceived of contentSize bytes came.
   */
  LoadingContent,
  /**
   * @brief The pack of the server could not be downloaded: the client left
   * the session, telling the server as when the player leaves, so its seat
   * is free again.
   */
  ContentDownloadFailed,
  /**
   * @brief The pack of the server came, but is not the one it announced, or
   * holds a sheet this client cannot read: it was never loaded, and the
   * client left the session as for ContentDownloadFailed.
   */
  ContentRefused,
};

}  // namespace rtype::client
