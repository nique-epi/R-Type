#pragma once

#include <cstdint>
#include <optional>
#include <string>
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
  /** @brief The first name of the table of the acceptance this client has no
   * sheet for; set when phase is MissingEntitySheet only. */
  std::optional<std::string> missingSheet = std::nullopt;
  /** @brief How many more names of that table it has no sheet for; 0 unless
   * phase is MissingEntitySheet. */
  std::uint8_t otherMissingSheets{};

  bool operator==(const ConnectionStatus&) const = default;
};

}  // namespace rtype::client
