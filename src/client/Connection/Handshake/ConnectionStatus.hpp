#pragma once

#include <cstddef>
#include <cstdint>
#include "ConnectionPhase.hpp"
#include "ConnectionRefused.hpp"

namespace rtype::client {

/** @brief Where an attempt to join a server stands, and what the server
 * answered. */
struct ConnectionStatus {
  ConnectionPhase phase = ConnectionPhase::Idle;
  /** @brief Number of the player, FIRST_PLAYER_NUMBER to MAXIMUM_PLAYERS;
   * meaningful when phase is Accepted only. */
  std::uint8_t playerNumber{};
  /** @brief Why the server refused; meaningful when phase is Refused only. */
  network::RefusalReason refusal{};
  /** @brief The protocol version the server writes in the header of its
   * answer; meaningful when refusal is IncompatibleVersion only. */
  std::uint8_t serverVersion{};
  /** @brief The bytes of the content pack received, and the bytes it holds;
   * meaningful when phase is LoadingContent only. */
  std::size_t contentReceived{};
  std::size_t contentSize{};

  bool operator==(const ConnectionStatus&) const = default;
};

}  // namespace rtype::client
