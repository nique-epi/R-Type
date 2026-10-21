#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace rtype::client {

/** @brief Where a pilot stands in the game, as the server last told it. */
enum class PilotState : std::uint8_t {
  /** @brief In the game. */
  Flying,
  /** @brief Its ship came back after a hit and cannot be hit for a while. */
  Respawning,
  /** @brief No life left: out of the game, its score kept. */
  OutOfLives,
  /** @brief Left the game. */
  Left,
  /** @brief Dropped by the server, which stopped hearing from it. */
  SignalLost,
};

/** @brief One pilot of the game: what the status band shows of it. */
struct PilotStatus {
  /** @brief FIRST_PLAYER_NUMBER to MAXIMUM_PLAYERS; its color is the number
   * less FIRST_PLAYER_NUMBER. */
  std::uint8_t playerNumber{};
  /** @brief The nickname the waiting room gave the pilot, UTF-8; empty when
   * the waiting room never listed it. */
  std::string nickname;
  std::uint32_t score{};
  /** @brief Nothing until the server lists the pilot in a world state. */
  std::optional<std::uint8_t> lives;
  PilotState state{};
  bool isLocalPlayer = false;

  bool operator==(const PilotStatus&) const = default;
};

}  // namespace rtype::client
