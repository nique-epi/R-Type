#pragma once

#include <cstdint>
#include "ConnectionRefused.hpp"

namespace rtype::client {

/** @brief Where an attempt to join a server stands, and what the server
 * answered. */
struct ConnectionStatus {
  /** @brief The steps of an attempt. */
  enum class Phase : std::uint8_t {
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
     * the checks of the session for SESSION_SILENCE_LIMIT: the client closed
     * the session without telling the server.
     */
    Lost,
  };

  Phase phase = Phase::Idle;
  /** @brief Number of the player, FIRST_PLAYER_NUMBER to MAXIMUM_PLAYERS;
   * meaningful when phase is Accepted only. */
  std::uint8_t playerNumber{};
  /** @brief Why the server refused; meaningful when phase is Refused only. */
  network::RefusalReason refusal{};
  /** @brief The protocol version the server writes in the header of its
   * answer; meaningful when refusal is IncompatibleVersion only. */
  std::uint8_t serverVersion{};

  bool operator==(const ConnectionStatus&) const = default;
};

}  // namespace rtype::client
